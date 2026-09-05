#include "balance.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float kSecondsPerDay = 86400.0f;
constexpr float kWaterDensity = 1000.0f;
constexpr float kMolarMassCarbon = 12.0f;
constexpr float kDefaultSaturation = 0.45f;
constexpr float kMinimumLambda = 2.0e6f;

float clampFinite(float value, float lower, float upper)
{
    if (!std::isfinite(value)) return lower;
    return std::clamp(value, lower, upper);
}

float meanPair(float first, float second)
{
    return 0.5f * (first + second);
}
}

WaterCarbonBalance::WaterCarbonBalance()
    : m_enabled(false),
      m_timeStepSeconds(3600.0f),
      m_rootDepthMeters(1.0f),
      m_specificLeafArea(0.02f),
      m_leafCarbonFraction(0.45f),
      m_leafAllocationFraction(0.40f),
      m_leafTurnoverPerDay(0.01f)
{
}

void WaterCarbonBalance::configure(bool enabled,
                                   float timeStepSeconds,
                                   float rootDepthMeters,
                                   float specificLeafArea,
                                   float leafCarbonFraction,
                                   float leafAllocationFraction,
                                   float leafTurnoverPerDay)
{
    m_enabled = enabled;
    m_timeStepSeconds = std::max(1.0f, timeStepSeconds);
    m_rootDepthMeters = std::max(0.05f, rootDepthMeters);
    m_specificLeafArea = std::max(1.0e-4f, specificLeafArea);
    m_leafCarbonFraction = std::clamp(leafCarbonFraction, 0.05f, 0.8f);
    m_leafAllocationFraction = std::clamp(leafAllocationFraction, 0.0f, 1.0f);
    m_leafTurnoverPerDay = std::clamp(leafTurnoverPerDay, 0.0f, 1.0f);
}

void WaterCarbonBalance::beginDay(const std::shared_ptr<PixelIO>& pixelio) const
{
    if (!m_enabled || !pixelio) return;
    BalanceState& balance = pixelio->m_pDynamicVariable->balance;
    balance.dailyEtMm = 0.0f;
    balance.dailyPrecipitationMm = 0.0f;
    balance.dailyRunoffMm = 0.0f;
    balance.dailyGrossAssimilationGc = 0.0f;
    balance.dailyPlantRespirationGc = 0.0f;
    balance.dailyNetAssimilationGc = 0.0f;
}

void WaterCarbonBalance::update(const std::shared_ptr<PixelIO>& pixelio) const
{
    if (!m_enabled || !pixelio || !pixelio->m_pInputset ||
        !pixelio->m_pDynamicVariable) return;

    InputSet& input = *pixelio->m_pInputset;
    DynamicVariable& dynamic = *pixelio->m_pDynamicVariable;
    BalanceState& balance = dynamic.balance;
    const int node = pixelio->k_node;
    if (node < 0 || node >= static_cast<int>(input.vMeteo.size())) return;

    const Meteo& meteo = input.vMeteo[node];
    const float saturation = input.soilset.satwater > 0.0f
                                 ? std::clamp(input.soilset.satwater, 0.05f, 0.95f)
                                 : kDefaultSaturation;
    const float rootDepthMm = m_rootDepthMeters * kWaterDensity;
    const float maximumStorageMm = saturation * rootDepthMm;

    if (!balance.initialized) {
        balance.soilWaterMm = clampFinite(input.soilset.SMC, 0.0f, saturation) * rootDepthMm;
        const float lai = std::max(0.0f, input.canopy.lai);
        balance.leafCarbonGc = lai / m_specificLeafArea * m_leafCarbonFraction;
        balance.initialized = true;
    }

    float latentHeat = 2.45e6f;
    const float meanSurfaceTemperature = input.canopy.type == 13
                                              ? meteo.ta
                                              : meanPair(dynamic.thermal.Tleafsunlit,
                                                         dynamic.thermal.Tsoilshaded);
    if (std::isfinite(meanSurfaceTemperature)) {
        latentHeat = std::max(kMinimumLambda,
                              (2.501f - 0.002361f * (meanSurfaceTemperature - 273.15f)) * 1.0e6f);
    }

    float latentHeatFlux = 0.0f;
    if (input.canopy.type == 13) {
        latentHeatFlux = (dynamic.heatflux.LEroofsunlit + dynamic.heatflux.LEroofshaded +
                          dynamic.heatflux.LEwallsunlit + dynamic.heatflux.LEwallshaded +
                          dynamic.heatflux.LEstreetsunlit + dynamic.heatflux.LEstreetshaded) / 6.0f;
    } else {
        const float lai = std::max(0.0f, input.canopy.lai);
        latentHeatFlux = meanPair(dynamic.heatflux.LEleafsunlit,
                                  dynamic.heatflux.LEleafshaded) * lai +
                         meanPair(dynamic.heatflux.LEsoilsunlit,
                                  dynamic.heatflux.LEsoilshaded);
    }

    // Positive LE is water loss.  Negative LE (dew/fog) is not counted as
    // consumption in this first bucket implementation.
    const float potentialEtMm = std::max(0.0f, latentHeatFlux) *
                                m_timeStepSeconds / latentHeat;
    const float precipitationMm = std::max(0.0f, meteo.precipitation);
    const float availableWaterMm = balance.soilWaterMm + precipitationMm;
    const float runoffMm = std::max(0.0f, availableWaterMm - maximumStorageMm);
    const float waterAfterRunoffMm = availableWaterMm - runoffMm;
    const float actualEtMm = std::min(potentialEtMm, std::max(0.0f, waterAfterRunoffMm));
    balance.soilWaterMm = std::clamp(waterAfterRunoffMm - actualEtMm,
                                     0.0f, maximumStorageMm);
    input.soilset.SMC = balance.soilWaterMm / rootDepthMm;

    const float lai = std::max(0.0f, input.canopy.lai);
    const float grossRate = meanPair(dynamic.biostate.grossAssimilationSunlit,
                                     dynamic.biostate.grossAssimilationShaded) * lai;
    const float respirationRate = meanPair(dynamic.biostate.respirationSunlit,
                                           dynamic.biostate.respirationShaded) * lai;
    const float conversion = m_timeStepSeconds * 1.0e-6f * kMolarMassCarbon;
    const float grossCarbonGc = std::max(0.0f, grossRate) * conversion;
    const float plantRespirationGc = std::max(0.0f, respirationRate) * conversion;
    const float netCarbonGc = (grossRate - respirationRate) * conversion;

    balance.etMm = actualEtMm;
    balance.precipitationMm = precipitationMm;
    balance.runoffMm = runoffMm;
    balance.grossAssimilationGc = grossCarbonGc;
    balance.plantRespirationGc = plantRespirationGc;
    balance.netAssimilationGc = netCarbonGc;

    balance.dailyEtMm += actualEtMm;
    balance.dailyPrecipitationMm += precipitationMm;
    balance.dailyRunoffMm += runoffMm;
    balance.dailyGrossAssimilationGc += grossCarbonGc;
    balance.dailyPlantRespirationGc += plantRespirationGc;
    balance.dailyNetAssimilationGc += netCarbonGc;

    // A simple leaf carbon pool closes the carbon balance at the canopy level.
    // It is intentionally conservative: only a configurable fraction of NPP
    // is allocated to leaves, and turnover removes leaf carbon every day.
    const float turnover = balance.leafCarbonGc * m_leafTurnoverPerDay *
                           m_timeStepSeconds / kSecondsPerDay;
    balance.leafCarbonGc = std::max(0.0f,
                                    balance.leafCarbonGc +
                                    m_leafAllocationFraction * netCarbonGc - turnover);
    input.canopy.lai = std::clamp(balance.leafCarbonGc * m_specificLeafArea /
                                   m_leafCarbonFraction, 0.0f, 25.0f);
}
