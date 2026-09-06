#pragma once

#include "pixelio.h"

// A generic crop phenology/LAI prior for long time-series runs. It is not a
// substitute for a calibrated crop model; its purpose is to keep crop pixels
// plausible between sparse LAI observations when management data are missing.
class Growth
{
public:
    Growth();

    void configure(bool enabled,
                   int defaultSowingDoy,
                   float baseTemperatureC,
                   float emergenceGdd,
                   float peakGdd,
                   float maturityGdd,
                   float maximumLai,
                   float minimumLai,
                   float soilWiltingPoint,
                   float soilFieldCapacity);

    void advance(const std::shared_ptr<PixelIO>& pixelio,
                 int year,
                 int doy,
                 float meanAirTemperatureC,
                 float soilMoisture) const;

    bool enabled() const { return m_enabled; }

private:
    static bool cropPixel(const PixelIO& pixelio);
    static float clampFinite(float value, float lower, float upper);
    int sowingDoyFor(const PixelIO& pixelio) const;

    bool m_enabled;
    int m_defaultSowingDoy;
    float m_baseTemperatureC;
    float m_emergenceGdd;
    float m_peakGdd;
    float m_maturityGdd;
    float m_maximumLai;
    float m_minimumLai;
    float m_soilWiltingPoint;
    float m_soilFieldCapacity;
};
