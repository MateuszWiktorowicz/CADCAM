#pragma once

#include "toolpath/core/Geometry.hpp"

#include <cmath>

namespace toolpath::sketch {

struct Arc2D {
    core::Point2D center{0.0, 0.0};
    double radiusMm{1.0};
    double startAngleRad{0.0};
    double endAngleRad{0.0};
    bool clockwise{false};

    [[nodiscard]] core::Point2D startPoint() const
    {
        return pointAtAngle(startAngleRad);
    }

    [[nodiscard]] core::Point2D endPoint() const
    {
        return pointAtAngle(endAngleRad);
    }

    [[nodiscard]] core::Point2D pointAtAngle(double angleRad) const
    {
        return center + core::Point2D{std::cos(angleRad) * radiusMm, std::sin(angleRad) * radiusMm};
    }

    [[nodiscard]] double sweepAngleRad() const
    {
        if (clockwise) {
            return normalizedPositiveAngle(startAngleRad - endAngleRad);
        }

        return normalizedPositiveAngle(endAngleRad - startAngleRad);
    }

    [[nodiscard]] double lengthMm() const
    {
        return radiusMm * sweepAngleRad();
    }

    [[nodiscard]] bool isDegenerate(double toleranceMm = 1e-6) const
    {
        return radiusMm <= toleranceMm || lengthMm() <= toleranceMm;
    }

private:
    [[nodiscard]] static constexpr double fullTurnRad()
    {
        return 6.28318530717958647692;
    }

    [[nodiscard]] static double normalizedPositiveAngle(double angleRad)
    {
        double normalized = std::fmod(angleRad, fullTurnRad());
        if (normalized < 0.0) {
            normalized += fullTurnRad();
        }
        return normalized;
    }
};

} // namespace toolpath::sketch
