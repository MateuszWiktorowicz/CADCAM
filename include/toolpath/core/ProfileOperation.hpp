#pragma once

#include "toolpath/core/Geometry.hpp"
#include "toolpath/core/Tool.hpp"
#include "toolpath/core/Toolpath.hpp"

namespace toolpath::core {

enum class CompensationSide {
    None,
    Outside,
    Inside
};

struct RectangleProfileInput {
    double widthMm{40.0};
    double heightMm{20.0};
    Tool tool{};
    CuttingParameters cutting{};
    CompensationSide compensation{CompensationSide::Outside};
};

struct ProfileInput {
    Polyline2D profile{};
    Tool tool{};
    CuttingParameters cutting{};
    CompensationSide compensation{CompensationSide::Outside};
};

class ProfileOperation {
public:
    [[nodiscard]] Toolpath generateRectangleProfile(const RectangleProfileInput& input) const;
    [[nodiscard]] Toolpath generateProfile(const ProfileInput& input) const;

private:
    [[nodiscard]] Polyline2D buildCompensatedRectangle(const RectangleProfileInput& input) const;
    [[nodiscard]] Polyline2D buildCompensatedProfile(const ProfileInput& input) const;
};

} // namespace toolpath::core
