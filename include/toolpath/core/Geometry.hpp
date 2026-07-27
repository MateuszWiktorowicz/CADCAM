#pragma once

#include <Eigen/Core>
#include <vector>

namespace toolpath::core {

using Point2D = Eigen::Vector2d;
using Point3D = Eigen::Vector3d;

struct Polyline2D {
    std::vector<Point2D> points;
    bool closed{false};
};

inline Point2D leftNormal(const Point2D& direction)
{
    return Point2D{-direction.y(), direction.x()};
}

} // namespace toolpath::core

