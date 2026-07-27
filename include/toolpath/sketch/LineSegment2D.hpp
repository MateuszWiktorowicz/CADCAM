#pragma once

#include "toolpath/core/Geometry.hpp"

#include <cmath>

namespace toolpath::sketch {

struct LineSegment2D {
    core::Point2D startPoint{0.0, 0.0};
    core::Point2D endPoint{0.0, 0.0};

    [[nodiscard]] double lengthMm() const
    {
        return (endPoint - startPoint).norm();
    }

    [[nodiscard]] bool isDegenerate(double toleranceMm = 1e-6) const
    {
        return lengthMm() <= toleranceMm;
    }
};

} // namespace toolpath::sketch
