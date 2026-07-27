#pragma once

#include "toolpath/core/Geometry.hpp"

#include <vector>

namespace toolpath::core {

enum class MoveType {
    Rapid,
    Plunge,
    Cut
};

struct ToolpathMove {
    MoveType type{MoveType::Rapid};
    Point3D position{0.0, 0.0, 0.0};
    double feedRateMmMin{0.0};
};

struct Toolpath {
    std::vector<ToolpathMove> moves;
};

} // namespace toolpath::core

