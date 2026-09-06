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
      m_leafTurnoverPerDay(0.01f),
      m_canopyStoragePerLai(0.20f),
      m_canopyStorageBase(0.0f),
      m_fieldCapacity(0.35f),
      m_drainageRate(0.20f),
      m_cropModelOwnsLai(false)
{
}

void WaterCarbonBalance::configure(bool enabled,
                                   float timeStepSeconds,
                                   float rootDepthMeters,
                                   float specificLeafArea,
                                   float leafCarbonFraction,
                                   float leafAllocationFraction,
                                   float leafTurnoverPerDay,
                                   float canopyStoragePerLai,
                                   float canopyStorageBase,
                                   float fieldCapacity,
                                   float drainageRate,
                                   bool cropModelOwnsLai)
{
    m_enabled = enabled;
    m_timeStepSeconds = std::max(1.0f, timeStepSeconds);
    m_rootDepthMeters = std::max(0.05f, rootDepthMeters);
    m_specificLeafArea = std::max(1.0e-4f, specificLeafArea);
    m_leafCarbonFraction = std::clamp(leafCarbonFraction, 0.05f, 0.8f);
    m_leafAllocationFraction = std::clamp(leafAllocationFraction, 0.0f, 1.0f);
    m_leafTurnoverPerDay = std::clamp(leafTurnoverPerDay, 0.0f, 1.0f);
    m_canopyStoragePerLai = std::max(0.0f, canopyStoragePerLai);
    m_canopyStorageBase = std::max(0.0f, canopyStorageBase);
    m_fieldCapacity = std::clamp(fieldCapacity, 0.05f, 0.95f);
    m_drainageRate = std::clamp(drainageRate, 0.0f, 1.0f);
    m_cropModelOwnsLai = cropModelOwnsLai;
}

void WaterCarbonBalance::scheduleSoilMoistureObservation(
    const std::shared_ptr<PixelIO>& pixelio,
    float volumetricSoilMoisture) const
{
    if (!m_enabled || !pixelio || !pixelio->m_pDynamicVariable ||
        !std::isfinite(volumetricSoilMoisture)) return;
    pixelio->m_pDynamicVariable->balance.pendingSoilMoisture =
        std::clamp(volumetricSoilMoisture, 0.0f, 1.0f);
}

void WaterCarbonBalance::beginDay(const std::shared_ptr<PixelIO>& pixelio) const
{
    if (!m_enabled || !pixelio) return;
    BalanceState& balance = pixelio->m_pDynamicVariable->balance;
    balance.dailyEtMm = 0.0f;
    balance.dailyPrecipitationMm = 0.0f;
    balance.dailyInterceptionMm = 0.0f;
    balance.dailyThroughfallMm = 0.0f;
    balance.dailyInfiltrationMm = 0.0f;
    balance.dailyRunoffMm = 0.0f;
    balance.dailyDrainageMm = 0.0f;
    balance.dailyAssimilationWaterIncrementMm = 0.0f;
    balance.assimilationWaterIncrementMm = 0.0f;
    balance.dailyGrossAssimilationGc = 0.0f;
    balance.dailyPlantRespirationGc = 0.0f;
    balance.dailyNetAssimilationGc = 0.0f;

    const float saturation = pixelio->m_pInputset->soilset.satwater > 0.0f
                                 ? std::clamp(pixelio->m_pInputset->soilset.satwater,
                                              0.05f, 0.95f)
                                 : kDefaultSaturation;
    const float rootDepthMm = m_rootDepthMeters * kWaterDensity;
    if (balance.pendingSoilMoisture >= 0.0f) {
        const float observedWaterMm =
            std::clamp(balance.pendingSoilMoisture, 0.0f, saturation) * rootDepthMm;
        if (balance.initialized) {
            const float correction = observedWaterMm - balance.soilWaterMm;
            balance.dailyAssimilationWaterIncrementMm += correction;
            balance.assimilationWaterIncrementMm = correction;
        }
        balance.soilWaterMm = observedWaterMm;
        balance.soilWaterForecastMm = observedWaterMm;
        balance.pendingSoilMoisture = -1.0f;
    }
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
    const float fieldCapacityStorageMm =
        std::min(maximumStorageMm, m_fieldCapacity * rootDepthMm);

    if (!balance.initialized) {
        balance.soilWaterMm = clampFinite(input.soilset.SMC, 0.0f, saturation) * rootDepthMm;
        balance.soilWaterForecastMm = balance.soilWaterMm;
        balance.canopyWaterMm = 0.0f;
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

    // Positive LE is water loss. Negative LE is not counted as consumption.
    const float potentialEtMm = std::max(0.0f, latentHeatFlux) *
                                m_timeStepSeconds / latentHeat;
    const float precipitationMm = std::max(0.0f, meteo.precipitation);

    // Simple canopy interception bucket. The land-cover/LAI dependence keeps
    // the scheme usable for forest and crop pixels without irrigation data.
    const bool hasCanopy = input.canopy.dist == 1 || input.canopy.dist == 2 ||
                           input.canopy.dist == 3 || input.canopy.type == 12 ||
                           input.canopy.type == 14;
    const float lai = std::max(0.0f, input.canopy.lai);
    const float canopyStorageMax = hasCanopy
                                       ? m_canopyStorageBase +
                                             m_canopyStoragePerLai * lai
                                       : 0.0f;
    const float previousCanopyWaterMm = balance.canopyWaterMm;
    const float canopyWithRain = previousCanopyWaterMm + precipitationMm;
    const float throughfallMm = std::max(0.0f, canopyWithRain - canopyStorageMax);
    const float interceptionMm = std::min(
        precipitationMm,
        std::max(0.0f, canopyStorageMax - previousCanopyWaterMm));
    balance.canopyWaterMm = std::clamp(canopyWithRain - throughfallMm,
                                       0.0f, canopyStorageMax);
    const float interceptionEtMm = std::min(balance.canopyWaterMm,
                                             potentialEtMm);
    balance.canopyWaterMm -= interceptionEtMm;

    const float soilStorageBeforeRain = balance.soilWaterMm;
    const float soilCapacityMm = std::max(0.0f,
                                          maximumStorageMm - soilStorageBeforeRain);
    const float infiltrationMm = std::min(throughfallMm, soilCapacityMm);
    const float runoffMm = std::max(0.0f, throughfallMm - infiltrationMm);
    const float soilWaterAfterRain = soilStorageBeforeRain + infiltrationMm;
    const float soilEtMm = std::min(std::max(0.0f, potentialEtMm - interceptionEtMm),
                                    soilWaterAfterRain);
    const float waterAfterEtMm = soilWaterAfterRain - soilEtMm;
    const float drainageMm = std::min(
        std::max(0.0f, waterAfterEtMm - fieldCapacityStorageMm),
        std::max(0.0f, waterAfterEtMm - fieldCapacityStorageMm) *
            std::clamp(m_drainageRate * m_timeStepSeconds / kSecondsPerDay,
                       0.0f, 1.0f));
    const float actualEtMm = interceptionEtMm + soilEtMm;
    balance.soilWaterMm = std::clamp(waterAfterEtMm - drainageMm,
                                     0.0f, maximumStorageMm);
    balance.soilWaterForecastMm = balance.soilWaterMm;
    input.soilset.SMC = balance.soilWaterMm / rootDepthMm;

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
    balance.interceptionMm = interceptionMm;
    balance.throughfallMm = throughfallMm;
    balance.infiltrationMm = infiltrationMm;
    balance.runoffMm = runoffMm;
    balance.drainageMm = drainageMm;
    balance.grossAssimilationGc = grossCarbonGc;
    balance.plantRespirationGc = plantRespirationGc;
    balance.netAssimilationGc = netCarbonGc;

    balance.dailyEtMm += actualEtMm;
    balance.dailyPrecipitationMm += precipitationMm;
    balance.dailyInterceptionMm += interceptionMm;
    balance.dailyThroughfallMm += throughfallMm;
    balance.dailyInfiltrationMm += infiltrationMm;
    balance.dailyRunoffMm += runoffMm;
    balance.dailyDrainageMm += drainageMm;
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
    if (!(m_cropModelOwnsLai && input.canopy.dist == 2)) {
        input.canopy.lai = std::clamp(balance.leafCarbonGc * m_specificLeafArea /
                                       m_leafCarbonFraction, 0.0f, 25.0f);
    }
}
