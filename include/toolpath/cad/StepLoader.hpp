#pragma once

#include <TopoDS_Shape.hxx>

#include <filesystem>

namespace toolpath::cad {

class StepLoader {
public:
    [[nodiscard]] TopoDS_Shape loadShape(const std::filesystem::path& path) const;
};

} // namespace toolpath::cad

