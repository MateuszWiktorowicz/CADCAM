#pragma once

#include "toolpath/sketch/SketchEntity2D.hpp"

#include <cstddef>
#include <utility>
#include <vector>

namespace toolpath::sketch {

struct Sketch2D {
    std::vector<SketchEntity2D> entities;

    void add(SketchEntity2D entity)
    {
        entities.push_back(std::move(entity));
    }

    void clear()
    {
        entities.clear();
    }

    [[nodiscard]] bool empty() const
    {
        return entities.empty();
    }

    [[nodiscard]] std::size_t size() const
    {
        return entities.size();
    }
};

} // namespace toolpath::sketch
