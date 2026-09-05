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
                   float leafTurnoverPerDay);

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
};
