#pragma once

#include "pixelio.h"

// Minimal bucket water balance and leaf-carbon balance.  This is deliberately
// separate from the radiation/energy-balance solver so it can remain disabled
// while the forcing data are being prepared.
class WaterCarbonBalance
{
public:
    WaterCarbonBalance();

    void configure(bool enabled,
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
                   bool cropModelOwnsLai);

    // Apply an intermittent observed/analysed soil-moisture state at the
    // beginning of the next daily balance step. Missing observations are not
    // treated as zero and the correction is reported for water closure.
    void scheduleSoilMoistureObservation(const std::shared_ptr<PixelIO>& pixelio,
                                         float volumetricSoilMoisture) const;

    void beginDay(const std::shared_ptr<PixelIO>& pixelio) const;
    void update(const std::shared_ptr<PixelIO>& pixelio) const;

private:
    bool m_enabled;
    float m_timeStepSeconds;
    float m_rootDepthMeters;
    float m_specificLeafArea;
    float m_leafCarbonFraction;
    float m_leafAllocationFraction;
    float m_leafTurnoverPerDay;
    float m_canopyStoragePerLai;
    float m_canopyStorageBase;
    float m_fieldCapacity;
    float m_drainageRate;
    bool m_cropModelOwnsLai;
};
