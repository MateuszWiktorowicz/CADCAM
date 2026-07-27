#pragma once

#include "toolpath/core/Tool.hpp"
#include "toolpath/core/Toolpath.hpp"

#include <string>

namespace toolpath::core {

class GCodeWriter {
public:
    [[nodiscard]] std::string writeGrblProgram(const Toolpath& toolpath, const Tool& tool) const;
};

} // namespace toolpath::core

