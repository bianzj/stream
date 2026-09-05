#include "enkf.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace
{
constexpr float kMinimumObservationError = 1.0e-6f;
constexpr float kMinimumVariance = 1.0e-10f;
constexpr float kMaximumLai = 25.0f;
constexpr float kMinimumSoilMoisture = 0.0f;
constexpr float kMaximumSoilMoisture = 1.0f;
}

EnsembleKalmanFilter::EnsembleKalmanFilter()
    : m_ensembleSize(20),
      m_laiObservationError(0.3f),
      m_soilMoistureObservationError(0.05f),
      m_laiProcessNoise(0.05f),
      m_soilMoistureProcessNoise(0.01f),
      m_generator(20250906U)
{
}

void EnsembleKalmanFilter::configure(int ensembleSize,
                                     float laiObservationError,
                                     float soilMoistureObservationError,
                                     float laiProcessNoise,
                                     float soilMoistureProcessNoise,
                                     unsigned int seed)
{
    m_ensembleSize = std::max(2, ensembleSize);
    m_laiObservationError = std::max(kMinimumObservationError, laiObservationError);
    m_soilMoistureObservationError = std::max(kMinimumObservationError,
                                               soilMoistureObservationError);
    m_laiProcessNoise = std::max(0.0f, laiProcessNoise);
    m_soilMoistureProcessNoise = std::max(0.0f, soilMoistureProcessNoise);
    m_generator.seed(seed);
    reset();
}

void EnsembleKalmanFilter::reset()
{
    m_pixels.clear();
}

void EnsembleKalmanFilter::ensurePixel(int pixelIndex)
{
    if (pixelIndex < 0) return;
    if (static_cast<std::size_t>(pixelIndex) >= m_pixels.size()) {
        m_pixels.resize(static_cast<std::size_t>(pixelIndex) + 1);
    }
}

float EnsembleKalmanFilter::clampFinite(float value, float lowerBound, float upperBound)
{
    if (!std::isfinite(value)) return lowerBound;
    return std::clamp(value, lowerBound, upperBound);
}

float EnsembleKalmanFilter::mean(const std::vector<float>& values)
{
    if (values.empty()) return 0.0f;
    double total = 0.0;
    for (float value : values) total += value;
    return static_cast<float>(total / static_cast<double>(values.size()));
}

void EnsembleKalmanFilter::predict(PixelEnsemble& ensemble,
                                   float laiForecast,
                                   float soilMoistureForecast)
{
    const float safeLaiForecast = clampFinite(laiForecast, 0.0f, kMaximumLai);
    const float safeSoilMoistureForecast = clampFinite(soilMoistureForecast,
                                                       kMinimumSoilMoisture,
                                                       kMaximumSoilMoisture);
    std::normal_distribution<float> laiNoise(0.0f, m_laiProcessNoise);
    std::normal_distribution<float> soilMoistureNoise(0.0f, m_soilMoistureProcessNoise);

    if (!ensemble.initialized) {
        ensemble.lai.resize(static_cast<std::size_t>(m_ensembleSize));
        ensemble.soilMoisture.resize(static_cast<std::size_t>(m_ensembleSize));
        for (int member = 0; member < m_ensembleSize; ++member) {
            ensemble.lai[member] = clampFinite(safeLaiForecast + laiNoise(m_generator),
                                               0.0f, kMaximumLai);
            ensemble.soilMoisture[member] = clampFinite(
                safeSoilMoistureForecast + soilMoistureNoise(m_generator),
                kMinimumSoilMoisture, kMaximumSoilMoisture);
        }
        ensemble.initialized = true;
        return;
    }

    // No rainfall/soil-water balance is available yet.  Until that module is
    // added, persistence plus process noise is the explicit forecast model.
    for (int member = 0; member < m_ensembleSize; ++member) {
        ensemble.lai[member] = clampFinite(ensemble.lai[member] + laiNoise(m_generator),
                                           0.0f, kMaximumLai);
        ensemble.soilMoisture[member] = clampFinite(
            ensemble.soilMoisture[member] + soilMoistureNoise(m_generator),
            kMinimumSoilMoisture, kMaximumSoilMoisture);
    }
}

void EnsembleKalmanFilter::updateScalar(PixelEnsemble& ensemble,
                                         float observation,
                                         float observationError,
                                         bool updateLai,
                                         float lowerBound,
                                         float upperBound)
{
    if (!std::isfinite(observation) || ensemble.lai.empty()) return;

    const std::vector<float>& observedState = updateLai ? ensemble.lai
                                                        : ensemble.soilMoisture;
    const float stateMean = mean(observedState);

    double variance = 0.0;
    for (float value : observedState) {
        const double delta = static_cast<double>(value - stateMean);
        variance += delta * delta;
    }
    variance /= static_cast<double>(std::max(1, m_ensembleSize - 1));
    const double observationVariance = std::max(
        static_cast<double>(kMinimumVariance),
        static_cast<double>(observationError) * static_cast<double>(observationError));
    const double denominator = variance + observationVariance;
    if (denominator <= 0.0) return;

    // Stochastic EnKF: perturb the scalar observation for each member.
    std::normal_distribution<float> observationNoise(0.0f, observationError);
    std::vector<float> innovation(m_ensembleSize, 0.0f);
    for (int member = 0; member < m_ensembleSize; ++member) {
        innovation[member] = observation + observationNoise(m_generator) - observedState[member];
    }

    const std::vector<float> priorLai = ensemble.lai;
    const std::vector<float> priorSoilMoisture = ensemble.soilMoisture;
    for (int member = 0; member < m_ensembleSize; ++member) {
        const double observedDelta = static_cast<double>(observedState[member] - stateMean);
        const double laiDelta = static_cast<double>(priorLai[member] - mean(priorLai));
        const double soilMoistureDelta = static_cast<double>(
            priorSoilMoisture[member] - mean(priorSoilMoisture));
        const double laiGain = laiDelta * observedDelta / denominator;
        const double soilMoistureGain = soilMoistureDelta * observedDelta / denominator;

        ensemble.lai[member] = clampFinite(
            priorLai[member] + static_cast<float>(laiGain * innovation[member]),
            lowerBound, upperBound);
        ensemble.soilMoisture[member] = clampFinite(
            priorSoilMoisture[member] + static_cast<float>(soilMoistureGain * innovation[member]),
            kMinimumSoilMoisture, kMaximumSoilMoisture);
    }
}

void EnsembleKalmanFilter::assimilate(int pixelIndex,
                                      float laiForecast,
                                      float soilMoistureForecast,
                                      float laiObservation,
                                      float soilMoistureObservation,
                                      bool hasLaiObservation,
                                      bool hasSoilMoistureObservation,
                                      float& laiAnalysis,
                                      float& soilMoistureAnalysis)
{
    ensurePixel(pixelIndex);
    if (pixelIndex < 0 || static_cast<std::size_t>(pixelIndex) >= m_pixels.size()) {
        laiAnalysis = laiForecast;
        soilMoistureAnalysis = soilMoistureForecast;
        return;
    }

    PixelEnsemble& ensemble = m_pixels[static_cast<std::size_t>(pixelIndex)];
    predict(ensemble, laiForecast, soilMoistureForecast);

    if (hasLaiObservation) {
        updateScalar(ensemble, laiObservation, m_laiObservationError,
                     true, 0.0f, kMaximumLai);
    }
    if (hasSoilMoistureObservation) {
        updateScalar(ensemble, soilMoistureObservation, m_soilMoistureObservationError,
                     false, kMinimumSoilMoisture, kMaximumSoilMoisture);
    }

    laiAnalysis = clampFinite(mean(ensemble.lai), 0.0f, kMaximumLai);
    soilMoistureAnalysis = clampFinite(mean(ensemble.soilMoisture),
                                       kMinimumSoilMoisture,
                                       kMaximumSoilMoisture);
}
