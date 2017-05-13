#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "data/hex_grid.h"

namespace asdf
{
namespace hexmap
{
namespace data
{
    using terrain_brush_mask_t = std::vector<bool>;

    struct terrain_brush_t
    {
        terrain_brush_mask_t brush_mask;

        terrain_brush_t() = default;
        terrain_brush_t(glm::uvec2 size, bool start_val = true);
        terrain_brush_t(terrain_brush_mask_t&&);

        //inline glm::uvec2 size() const;
    };

    terrain_brush_t terrain_brush_circle(float radius);
}
}
}