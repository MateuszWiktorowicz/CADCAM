#pragma once

#include "toolpath/core/Geometry.hpp"

#include <gp_Ax3.hxx>

namespace toolpath::cad {

struct AxisSegment3D {
    core::Point3D start{0.0, 0.0, 0.0};
    core::Point3D end{0.0, 0.0, 0.0};
};

class CartesianCoordinateSystem {
public:
    explicit CartesianCoordinateSystem(gp_Ax3 axes = gp_Ax3{});

    [[nodiscard]] const gp_Ax3& axes() const;
    [[nodiscard]] core::Point3D origin() const;
    [[nodiscard]] AxisSegment3D xAxis(double lengthMm) const;
    [[nodiscard]] AxisSegment3D yAxis(double lengthMm) const;
    [[nodiscard]] AxisSegment3D zAxis(double lengthMm) const;

private:
    [[nodiscard]] AxisSegment3D axisSegment(const gp_Dir& direction, double lengthMm) const;

    gp_Ax3 axes_;
};

} // namespace toolpath::cad
