#pragma once

#include "toolpath/core/Geometry.hpp"

#include <cmath>

namespace toolpath::sketch {

struct Circle2D {
    core::Point2D center{0.0, 0.0};
    double radiusMm{1.0};

    [[nodiscard]] double diameterMm() const
    {
        return radiusMm * 2.0;
    }

    [[nodiscard]] double circumferenceMm() const
    {
        return 2.0 * pi() * radiusMm;
    }

    [[nodiscard]] double areaMm2() const
    {
        return pi() * radiusMm * radiusMm;
    }

    [[nodiscard]] core::Point2D pointAtAngle(double angleRad) const
    {
        return center + core::Point2D{std::cos(angleRad) * radiusMm, std::sin(angleRad) * radiusMm};
    }

    [[nodiscard]] bool isDegenerate(double toleranceMm = 1e-6) const
    {
        return radiusMm <= toleranceMm;
    }

private:
    [[nodiscard]] static constexpr double pi()
    {
        return 3.14159265358979323846;
    }
};

} // namespace toolpath::sketch
