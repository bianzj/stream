#include "radiative_lai.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float kMinimumCosine = 0.01f;
constexpr float kMinimumGap = 1.0e-12f;
constexpr int kHemisphereSamples = 90;

float safeCosine(float zenithDeg)
{
    const float boundedZenith = std::clamp(zenithDeg, 0.0f, 89.0f);
    return std::max(static_cast<float>(std::cos(boundedZenith * RD)),
                    kMinimumCosine);
}
}

bool RadiativeLai::hasCrownStructure(const Canopy& canopy)
{
    return canopy.type >= 1 && canopy.type <= 5 &&
           canopy.lai > 0.0f && canopy.Gleaf > 0.0f &&
           canopy.treeStand > 0.0f && canopy.canopyHeight > 0.0f &&
           canopy.b > 0.0f;
}

bool RadiativeLai::sameScene(const Canopy& lhs, const Canopy& rhs)
{
    return lhs.lai == rhs.lai && lhs.type == rhs.type &&
           lhs.Gleaf == rhs.Gleaf && lhs.treeStand == rhs.treeStand &&
           lhs.canopyHeight == rhs.canopyHeight && lhs.b == rhs.b;
}

void RadiativeLai::updateSceneCache(EffectiveLaiCache& cache, const Canopy& canopy)
{
    if (cache.sceneValid && sameScene(cache.sourceCanopy, canopy)) {
        return;
    }

    cache.sourceCanopy = canopy;
    cache.values.hemispherical = hemispherical(canopy);
    cache.sceneValid = true;
    cache.viewingValid = false;
}

float RadiativeLai::directionalGap(const Canopy& canopy, float zenithDeg)
{
    const float lai = std::max(canopy.lai, 0.0f);
    if (lai <= 0.0f) {
        return 1.0f;
    }

    const float projection = canopy.Gleaf > 0.0f ? canopy.Gleaf : GG;
    const float cosine = safeCosine(zenithDeg);
    if (!hasCrownStructure(canopy)) {
        return std::clamp(std::exp(-projection * lai / cosine), kMinimumGap, 1.0f);
    }

    const float boundedZenith = std::clamp(zenithDeg, 0.0f, 89.0f);
    const double theta = boundedZenith * RD;
    const double verticalRadius = canopy.canopyHeight / 2.0;
    const double horizontalRadius = canopy.canopyHeight / (2.0 * canopy.b);
    const double crownArea =
        std::sqrt(verticalRadius * verticalRadius +
                  std::pow(horizontalRadius * std::tan(theta), 2.0)) *
        PI * verticalRadius;

    if (!(crownArea > 0.0) || !std::isfinite(crownArea)) {
        return std::clamp(std::exp(-projection * lai / cosine), kMinimumGap, 1.0f);
    }

    const double insideExponent =
        -static_cast<double>(lai) * projection /
        (crownArea * canopy.treeStand * cosine);
    const double gapInside = std::exp(std::max(insideExponent, -80.0));
    const double gap = std::exp(-crownArea * (1.0 - gapInside) * canopy.treeStand);
    return std::clamp(static_cast<float>(gap), kMinimumGap, 1.0f);
}

float RadiativeLai::directional(const Canopy& canopy, float zenithDeg)
{
    const float lai = std::max(canopy.lai, 0.0f);
    if (!hasCrownStructure(canopy)) {
        return lai;
    }

    const float projection = canopy.Gleaf > 0.0f ? canopy.Gleaf : GG;
    const float gap = directionalGap(canopy, zenithDeg);
    const float effectiveLai = -std::log(gap) * safeCosine(zenithDeg) / projection;
    return std::clamp(effectiveLai, 0.0f, lai);
}

float RadiativeLai::hemispherical(const Canopy& canopy)
{
    const float lai = std::max(canopy.lai, 0.0f);
    if (!hasCrownStructure(canopy)) {
        return lai;
    }

    double weightedLai = 0.0;
    double weightSum = 0.0;
    for (int sample = 0; sample < kHemisphereSamples; ++sample) {
        const double theta = (sample + 0.5) * (0.5 * PI / kHemisphereSamples);
        const double weight = 2.0 * std::cos(theta) * std::sin(theta);
        weightedLai += directional(canopy, static_cast<float>(theta / RD)) * weight;
        weightSum += weight;
    }

    return std::clamp(static_cast<float>(weightedLai / weightSum), 0.0f, lai);
}

EffectiveLai RadiativeLai::calculate(const Canopy& canopy, float szaDeg, float vzaDeg)
{
    return EffectiveLai{
        hemispherical(canopy),
        directional(canopy, szaDeg),
        directional(canopy, vzaDeg)
    };
}

EffectiveLai RadiativeLai::calculateCached(EffectiveLaiCache& cache,
                                           const Canopy& canopy,
                                           float szaDeg,
                                           float vzaDeg)
{
    updateSceneCache(cache, canopy);
    cache.values.solar = directional(canopy, szaDeg);

    if (!cache.viewingValid || cache.viewingZenith != vzaDeg) {
        cache.values.viewing = directional(canopy, vzaDeg);
        cache.viewingZenith = vzaDeg;
        cache.viewingValid = true;
    }
    return cache.values;
}

float RadiativeLai::hemisphericalCached(EffectiveLaiCache& cache,
                                        const Canopy& canopy)
{
    updateSceneCache(cache, canopy);
    return cache.values.hemispherical;
}
