#include "image_simulator.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float kMinCosine = 0.05f;

float safeCosine(float angleDeg)
{
    const float cosine = std::cos(angleDeg * RD);
    return std::max(cosine, kMinCosine);
}

}

float ImageSimulator::clamp01(float value)
{
    return std::clamp(value, 0.0f, 1.0f);
}

ImageSimulationResult ImageSimulator::simulate(const Canopy& canopy,
                                               const Thermal& thermal,
                                               const Spectral& spectral,
                                               const AtomCond& atomcond,
                                               const Meteo& meteo,
                                               const Angle& angle,
                                               float tirWavelengthUm) const
{
    ImageSimulationResult result;

    const EffectiveLai effectiveLai =
        RadiativeLai::calculate(canopy, angle.sza, angle.vza);
    const float projection = canopy.Gleaf > 0.0f ? canopy.Gleaf : GG;
    const float sunCosine = safeCosine(angle.sza);
    const bool sunAboveHorizon = angle.sza < 90.0f;
    const float directTransmittance =
        std::exp(-projection * effectiveLai.solar / sunCosine);
    const float diffuseTransmittance =
        std::exp(-0.825f * projection * effectiveLai.hemispherical);
    const float incomingRadiation = std::max(meteo.rin, 0.0f) + std::max(meteo.rli, 0.0f);

    result.optical.wavelength_nm.resize(N1);
    result.optical.leafRadiance.assign(N1, 0.0f);
    result.optical.soilRadiance.assign(N1, 0.0f);
    result.optical.totalRadiance.assign(N1, 0.0f);
    result.optical.totalReflectance.assign(N1, 0.0f);
    result.optical.leafAbsorbed.assign(N1, 0.0f);
    result.optical.soilAbsorbed.assign(N1, 0.0f);

    for (int band = 0; band < N1; ++band) {
        const float wavelength = spectral.wl_[band] > 0.0f
                                     ? spectral.wl_[band]
                                     : atomcond.wl[band];
        const float direct = sunAboveHorizon
            ? std::max(meteo.rin, 0.0f) * atomcond.fesun[band] * 0.001f
            : 0.0f;
        const float diffuse = std::max(meteo.rli, 0.0f) * atomcond.fesky[band] * 0.001f;
        const float leafReflectance = clamp01(spectral.leafRefl_[band]);
        const float leafTransmittance = clamp01(spectral.leafTran_[band]);
        const float soilReflectance = clamp01(spectral.soilRefl_[band]);
        const float leafAbsorptance = clamp01(1.0f - leafReflectance - leafTransmittance);
        const float soilAbsorptance = 1.0f - soilReflectance;

        const float directLeafInteraction = 1.0f - directTransmittance;
        const float diffuseLeafInteraction = 1.0f - diffuseTransmittance;
        const float reflectedByLeaf = leafReflectance *
            (direct * directLeafInteraction + diffuse * diffuseLeafInteraction);
        const float reflectedBySoil = soilReflectance *
            (direct * directTransmittance + diffuse * diffuseTransmittance);
        const float reflected = reflectedByLeaf + reflectedBySoil;

        result.optical.wavelength_nm[band] = wavelength;
        result.optical.leafRadiance[band] = reflectedByLeaf / PI;
        result.optical.soilRadiance[band] = reflectedBySoil / PI;
        result.optical.totalRadiance[band] = reflected / PI;
        result.optical.totalReflectance[band] = incomingRadiation > 0.0f
            ? reflected / incomingRadiation
            : 0.0f;
        result.optical.leafAbsorbed[band] = leafAbsorptance *
            (direct * directLeafInteraction + diffuse * diffuseLeafInteraction);
        result.optical.soilAbsorbed[band] = soilAbsorptance *
            (direct * directTransmittance + diffuse * diffuseTransmittance);
    }

    const float viewGap = std::exp(-projection * effectiveLai.viewing /
                                   safeCosine(angle.vza));
    const float sunGap = sunAboveHorizon
        ? std::exp(-projection * effectiveLai.solar / sunCosine)
        : 1.0f;
    const float soilVisible = clamp01(viewGap);
    const float leafVisible = 1.0f - soilVisible;
    const float soilSunlit = soilVisible * (1.0f - sunGap);
    const float leafSunlit = leafVisible * (1.0f - sunGap);

    result.infrared.wavelength_um = tirWavelengthUm;
    result.infrared.soilSunlitFraction = clamp01(soilSunlit);
    result.infrared.soilShadedFraction = clamp01(soilVisible - soilSunlit);
    result.infrared.leafSunlitFraction = clamp01(leafSunlit);
    result.infrared.leafShadedFraction = clamp01(leafVisible - leafSunlit);

    const float soilEmissivity = clamp01(1.0f - spectral.soilRefl_ir);
    const float leafEmissivity = clamp01(1.0f - spectral.leafRefl_ir - spectral.leafTran_ir);
    result.infrared.radiance =
        result.infrared.soilSunlitFraction * soilEmissivity * SCI::Planck(tirWavelengthUm, thermal.Tsoilsunlit) +
        result.infrared.soilShadedFraction * soilEmissivity * SCI::Planck(tirWavelengthUm, thermal.Tsoilshaded) +
        result.infrared.leafSunlitFraction * leafEmissivity * SCI::Planck(tirWavelengthUm, thermal.Tleafsunlit) +
        result.infrared.leafShadedFraction * leafEmissivity * SCI::Planck(tirWavelengthUm, thermal.Tleafshaded);

    if (result.infrared.radiance > 0.0f && std::isfinite(result.infrared.radiance)) {
        result.infrared.brightnessTemperature =
            SCI::invPlanck(tirWavelengthUm, result.infrared.radiance);
    }

    return result;
}
