#pragma once

#include "structs.h"

struct EffectiveLai
{
    float hemispherical = 0.0f;
    float solar = 0.0f;
    float viewing = 0.0f;
};

struct EffectiveLaiCache
{
    EffectiveLai values{};
    Canopy sourceCanopy{};
    float viewingZenith = 0.0f;
    bool sceneValid = false;
    bool viewingValid = false;
};

// Computes the three geometry-specific effective LAI values from one canopy
// structure. Hemispherical LAI is scene-only, solar LAI varies with SZA, and
// viewing LAI varies only when the observation zenith changes.
class RadiativeLai
{
public:
    static EffectiveLai calculate(const Canopy& canopy, float szaDeg, float vzaDeg);
    static EffectiveLai calculateCached(EffectiveLaiCache& cache,
                                        const Canopy& canopy,
                                        float szaDeg,
                                        float vzaDeg);
    static float hemisphericalCached(EffectiveLaiCache& cache,
                                     const Canopy& canopy);
    static float hemispherical(const Canopy& canopy);
    static float directional(const Canopy& canopy, float zenithDeg);
    static float directionalGap(const Canopy& canopy, float zenithDeg);

private:
    static bool hasCrownStructure(const Canopy& canopy);
    static bool sameScene(const Canopy& lhs, const Canopy& rhs);
    static void updateSceneCache(EffectiveLaiCache& cache, const Canopy& canopy);
};
