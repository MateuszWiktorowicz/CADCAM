#pragma once

#include "toolpath/sketch/Arc2D.hpp"
#include "toolpath/sketch/Circle2D.hpp"
#include "toolpath/sketch/LineSegment2D.hpp"

#include <variant>

namespace toolpath::sketch {

using SketchEntity2D = std::variant<LineSegment2D, Circle2D, Arc2D>;

} // namespace toolpath::sketch
