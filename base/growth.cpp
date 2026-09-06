#include "growth.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float kDefaultBiomassEfficiency = 1.0f;
}

Growth::Growth()
    : m_enabled(false),
      m_defaultSowingDoy(90),
      m_baseTemperatureC(5.0f),
      m_emergenceGdd(100.0f),
      m_peakGdd(800.0f),
      m_maturityGdd(1500.0f),
      m_maximumLai(4.5f),
      m_minimumLai(0.02f),
      m_soilWiltingPoint(0.10f),
      m_soilFieldCapacity(0.35f)
{
}

void Growth::configure(bool enabled,
                       int defaultSowingDoy,
                       float baseTemperatureC,
                       float emergenceGdd,
                       float peakGdd,
                       float maturityGdd,
                       float maximumLai,
                       float minimumLai,
                       float soilWiltingPoint,
                       float soilFieldCapacity)
{
    m_enabled = enabled;
    m_defaultSowingDoy = std::clamp(defaultSowingDoy, 1, 365);
    m_baseTemperatureC = baseTemperatureC;
    m_emergenceGdd = std::max(0.0f, emergenceGdd);
    m_peakGdd = std::max(m_emergenceGdd + 1.0f, peakGdd);
    m_maturityGdd = std::max(m_peakGdd + 1.0f, maturityGdd);
    m_maximumLai = std::clamp(maximumLai, 0.1f, 25.0f);
    m_minimumLai = std::clamp(minimumLai, 0.0f, m_maximumLai);
    m_soilWiltingPoint = std::clamp(soilWiltingPoint, 0.0f, 1.0f);
    m_soilFieldCapacity = std::clamp(soilFieldCapacity,
                                     m_soilWiltingPoint + 1.0e-4f, 1.0f);
}

bool Growth::cropPixel(const PixelIO& pixelio)
{
    const int type = pixelio.m_pInputset->canopy.type;
    const int dist = pixelio.m_pInputset->canopy.dist;
    return dist == 2 || type == 12 || type == 14 ||
           pixelio.m_pDynamicVariable->crop.isCrop;
}

float Growth::clampFinite(float value, float lower, float upper)
{
    if (!std::isfinite(value)) return lower;
    return std::clamp(value, lower, upper);
}

int Growth::sowingDoyFor(const PixelIO& pixelio) const
{
    // A modest hemisphere shift provides a usable default without requiring
    // a local crop calendar. Missing latitude falls back to the configured
    // northern-hemisphere date.
    const float latitude = pixelio.lat;
    if (std::isfinite(latitude) && latitude < -5.0f) {
        return std::clamp(m_defaultSowingDoy + 182, 1, 365);
    }
    return m_defaultSowingDoy;
}

void Growth::advance(const std::shared_ptr<PixelIO>& pixelio,
                     int year,
                     int doy,
                     float meanAirTemperatureC,
                     float soilMoisture) const
{
    if (!m_enabled || !pixelio || !pixelio->m_pInputset ||
        !pixelio->m_pDynamicVariable || !cropPixel(*pixelio)) {
        return;
    }

    CropState& state = pixelio->m_pDynamicVariable->crop;
    state.isCrop = true;
    const bool newYear = !state.initialized || state.lastYear != year ||
                         doy < state.lastDoy;
    if (newYear) {
        state.initialized = true;
        state.lastYear = year;
        state.lastDoy = 0;
        state.sowingDoy = sowingDoyFor(*pixelio);
        state.gdd = 0.0f;
        state.stage = 0.0f;
        state.biomassGc = 0.0f;
        state.lai = m_minimumLai;
    }

    // The selected LAI from the previous day may be an observation or an
    // EnKF analysis. Keep the generic prior anchored between observations.
    const float selectedLai = clampFinite(pixelio->m_pInputset->canopy.lai,
                                          0.0f, m_maximumLai);
    if (state.lastDoy > 0 && std::isfinite(selectedLai)) {
        state.lai = selectedLai;
    }

    if (doy < state.sowingDoy) {
        state.gdd = 0.0f;
        state.stage = 0.0f;
        state.lai = m_minimumLai;
        pixelio->m_pInputset->canopy.lai = state.lai;
        state.lastDoy = doy;
        return;
    }

    const float dailyGdd = std::max(0.0f,
                                   clampFinite(meanAirTemperatureC,
                                                -80.0f, 80.0f) -
                                       m_baseTemperatureC);
    state.gdd = std::min(m_maturityGdd, state.gdd + dailyGdd);

    const float waterStress = clampFinite(
        (clampFinite(soilMoisture, m_soilWiltingPoint, m_soilFieldCapacity) -
         m_soilWiltingPoint) /
            (m_soilFieldCapacity - m_soilWiltingPoint),
        0.15f, 1.0f);

    if (state.gdd <= m_emergenceGdd) {
        state.stage = state.gdd / std::max(m_emergenceGdd, 1.0f);
        state.lai = m_minimumLai;
    } else if (state.gdd < m_peakGdd) {
        const float progress = (state.gdd - m_emergenceGdd) /
                               (m_peakGdd - m_emergenceGdd);
        const float smooth = progress * progress * (3.0f - 2.0f * progress);
        state.stage = 0.25f + 0.50f * progress;
        const float potentialLai = m_minimumLai +
                                   (m_maximumLai - m_minimumLai) * smooth;
        state.lai += (potentialLai - state.lai) *
                     (0.20f + 0.80f * waterStress);
    } else {
        const float progress = std::clamp(
            (state.gdd - m_peakGdd) / (m_maturityGdd - m_peakGdd),
            0.0f, 1.0f);
        state.stage = 0.75f + 0.25f * progress;
        const float potentialLai = m_maximumLai * (1.0f - progress) +
                                   m_minimumLai;
        state.lai += (potentialLai - state.lai) * 0.45f;
    }

    if (state.gdd >= m_maturityGdd) {
        state.stage = 1.0f;
        state.lai = m_minimumLai;
    }

    // A bounded carbon proxy is available for future product output, but it
    // does not overwrite TIRT's photosynthesis fluxes.
    state.biomassGc += std::max(0.0f, state.lai) * waterStress *
                       kDefaultBiomassEfficiency;
    state.lai = clampFinite(state.lai, m_minimumLai, m_maximumLai);
    pixelio->m_pInputset->canopy.lai = state.lai;
    state.lastDoy = doy;
}
