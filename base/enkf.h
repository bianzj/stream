#pragma once

#include <random>
#include <vector>

// A small, serial stochastic EnKF for the two prognostic variables that are
// available in the current STREAM input pipeline: LAI and volumetric soil
// moisture.  It is intentionally independent from the energy-balance model.
// The caller decides whether to use the analysis or keep the original inputs.
class EnsembleKalmanFilter
{
public:
    EnsembleKalmanFilter();

    void configure(int ensembleSize,
                   float laiObservationError,
                   float soilMoistureObservationError,
                   float laiProcessNoise,
                   float soilMoistureProcessNoise,
                   unsigned int seed = 20250906U);

    void reset();

    // Forecast is supplied by the host model on the first call.  Afterwards
    // the filter carries its last analysis forward with process noise.  Each
    // valid observation is assimilated serially, so LAI and soil moisture do
    // not need to arrive at the same time.
    void assimilate(int pixelIndex,
                    float laiForecast,
                    float soilMoistureForecast,
                    float laiObservation,
                    float soilMoistureObservation,
                    bool hasLaiObservation,
                    bool hasSoilMoistureObservation,
                    float& laiAnalysis,
                    float& soilMoistureAnalysis);

private:
    struct PixelEnsemble
    {
        std::vector<float> lai;
        std::vector<float> soilMoisture;
        bool initialized = false;
    };

    void ensurePixel(int pixelIndex);
    void predict(PixelEnsemble& ensemble,
                 float laiForecast,
                 float soilMoistureForecast);
    void updateScalar(PixelEnsemble& ensemble,
                      float observation,
                      float observationError,
                      bool updateLai,
                      float lowerBound,
                      float upperBound);
    static float mean(const std::vector<float>& values);
    static float clampFinite(float value, float lowerBound, float upperBound);

    int m_ensembleSize;
    float m_laiObservationError;
    float m_soilMoistureObservationError;
    float m_laiProcessNoise;
    float m_soilMoistureProcessNoise;
    std::mt19937 m_generator;
    std::vector<PixelEnsemble> m_pixels;
};
