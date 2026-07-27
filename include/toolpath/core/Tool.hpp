#pragma once

namespace toolpath::core {

struct Tool {
    double diameterMm{3.175};
    double spindleRpm{12000.0};

    [[nodiscard]] double radiusMm() const
    {
        return diameterMm * 0.5;
    }
};

struct CuttingParameters {
    double feedRateMmMin{300.0};
    double plungeRateMmMin{120.0};
    double safeZMm{5.0};
    double cutDepthMm{-2.0};
    double stepDownMm{0.5};
};

} // namespace toolpath::core

