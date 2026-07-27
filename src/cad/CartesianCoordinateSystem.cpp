#include "toolpath/cad/CartesianCoordinateSystem.hpp"

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

namespace toolpath::cad {

namespace {

[[nodiscard]] core::Point3D toPoint3D(const gp_Pnt& point)
{
    return core::Point3D{point.X(), point.Y(), point.Z()};
}

} // namespace

CartesianCoordinateSystem::CartesianCoordinateSystem(gp_Ax3 axes)
    : axes_(std::move(axes))
{
}

const gp_Ax3& CartesianCoordinateSystem::axes() const
{
    return axes_;
}

core::Point3D CartesianCoordinateSystem::origin() const
{
    return toPoint3D(axes_.Location());
}

AxisSegment3D CartesianCoordinateSystem::xAxis(double lengthMm) const
{
    return axisSegment(axes_.XDirection(), lengthMm);
}

AxisSegment3D CartesianCoordinateSystem::yAxis(double lengthMm) const
{
    return axisSegment(axes_.YDirection(), lengthMm);
}

AxisSegment3D CartesianCoordinateSystem::zAxis(double lengthMm) const
{
    return axisSegment(axes_.Direction(), lengthMm);
}

AxisSegment3D CartesianCoordinateSystem::axisSegment(const gp_Dir& direction, double lengthMm) const
{
    const auto start = origin();
    const core::Point3D offset{
        direction.X() * lengthMm,
        direction.Y() * lengthMm,
        direction.Z() * lengthMm
    };

    return AxisSegment3D{start, start + offset};
}

} // namespace toolpath::cad
