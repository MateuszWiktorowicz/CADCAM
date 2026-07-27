#include "toolpath/core/ProfileOperation.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace toolpath::core {

Toolpath ProfileOperation::generateRectangleProfile(const RectangleProfileInput& input) const
{
    if (input.widthMm <= 0.0 || input.heightMm <= 0.0) {
        throw std::invalid_argument("Rectangle dimensions must be positive.");
    }
    if (input.tool.diameterMm <= 0.0) {
        throw std::invalid_argument("Tool diameter must be positive.");
    }
    if (input.cutting.stepDownMm <= 0.0) {
        throw std::invalid_argument("Stepdown must be positive.");
    }
    if (input.cutting.cutDepthMm >= 0.0) {
        throw std::invalid_argument("Cut depth must be negative in machine coordinates.");
    }

    const auto contour = buildCompensatedRectangle(input);
    const auto start = contour.points.front();

    Toolpath result;
    result.moves.push_back({MoveType::Rapid, Point3D{start.x(), start.y(), input.cutting.safeZMm}, 0.0});

    const double targetDepth = input.cutting.cutDepthMm;
    double currentDepth = 0.0;

    while (currentDepth > targetDepth) {
        currentDepth = std::max(currentDepth - input.cutting.stepDownMm, targetDepth);
        result.moves.push_back({MoveType::Plunge, Point3D{start.x(), start.y(), currentDepth}, input.cutting.plungeRateMmMin});

        for (std::size_t i = 1; i < contour.points.size(); ++i) {
            const auto& point = contour.points[i];
            result.moves.push_back({MoveType::Cut, Point3D{point.x(), point.y(), currentDepth}, input.cutting.feedRateMmMin});
        }
    }

    result.moves.push_back({MoveType::Rapid, Point3D{start.x(), start.y(), input.cutting.safeZMm}, 0.0});
    return result;
}

Toolpath ProfileOperation::generateProfile(const ProfileInput& input) const
{
    if (!input.profile.closed || input.profile.points.size() < 4) {
        throw std::invalid_argument("Profile must be a closed polyline.");
    }
    if (input.tool.diameterMm <= 0.0) {
        throw std::invalid_argument("Tool diameter must be positive.");
    }
    if (input.cutting.stepDownMm <= 0.0) {
        throw std::invalid_argument("Stepdown must be positive.");
    }
    if (input.cutting.cutDepthMm >= 0.0) {
        throw std::invalid_argument("Cut depth must be negative in machine coordinates.");
    }

    const auto contour = buildCompensatedProfile(input);
    const auto start = contour.points.front();

    Toolpath result;
    result.moves.push_back({MoveType::Rapid, Point3D{start.x(), start.y(), input.cutting.safeZMm}, 0.0});

    const double targetDepth = input.cutting.cutDepthMm;
    double currentDepth = 0.0;

    while (currentDepth > targetDepth) {
        currentDepth = std::max(currentDepth - input.cutting.stepDownMm, targetDepth);
        result.moves.push_back({MoveType::Plunge, Point3D{start.x(), start.y(), currentDepth}, input.cutting.plungeRateMmMin});

        for (std::size_t i = 1; i < contour.points.size(); ++i) {
            const auto& point = contour.points[i];
            result.moves.push_back({MoveType::Cut, Point3D{point.x(), point.y(), currentDepth}, input.cutting.feedRateMmMin});
        }
    }

    result.moves.push_back({MoveType::Rapid, Point3D{start.x(), start.y(), input.cutting.safeZMm}, 0.0});
    return result;
}

Polyline2D ProfileOperation::buildCompensatedRectangle(const RectangleProfileInput& input) const
{
    const double halfWidth = input.widthMm * 0.5;
    const double halfHeight = input.heightMm * 0.5;
    const double radius = input.tool.radiusMm();

    double offset = 0.0;
    if (input.compensation == CompensationSide::Outside) {
        offset = radius;
    } else if (input.compensation == CompensationSide::Inside) {
        offset = -radius;
    }

    const double x = halfWidth + offset;
    const double y = halfHeight + offset;
    if (x <= 0.0 || y <= 0.0) {
        throw std::invalid_argument("Tool is too large for inside compensation.");
    }

    return Polyline2D{
        {
            Point2D{-x, -y},
            Point2D{x, -y},
            Point2D{x, y},
            Point2D{-x, y},
            Point2D{-x, -y},
        },
        true
    };
}

Polyline2D ProfileOperation::buildCompensatedProfile(const ProfileInput& input) const
{
    double minX = input.profile.points.front().x();
    double maxX = minX;
    double minY = input.profile.points.front().y();
    double maxY = minY;

    for (const auto& point : input.profile.points) {
        minX = std::min(minX, point.x());
        maxX = std::max(maxX, point.x());
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }

    const double radius = input.tool.radiusMm();
    double offset = 0.0;
    if (input.compensation == CompensationSide::Outside) {
        offset = radius;
    } else if (input.compensation == CompensationSide::Inside) {
        offset = -radius;
    }

    minX -= offset;
    maxX += offset;
    minY -= offset;
    maxY += offset;

    if (minX >= maxX || minY >= maxY) {
        throw std::invalid_argument("Tool is too large for inside compensation.");
    }

    return Polyline2D{
        {
            Point2D{minX, minY},
            Point2D{maxX, minY},
            Point2D{maxX, maxY},
            Point2D{minX, maxY},
            Point2D{minX, minY},
        },
        true
    };
}

} // namespace toolpath::core
