#pragma once

#include <glm\glm.hpp>

#include "asdf_multiplat/ui/polygon.h"
#include "asdf_multiplat/ui/ui_base.h"

#include "data/hex_grid.h"


using color_t = glm::vec4;

namespace asdf
{
namespace hexmap
{
namespace ui
{
    struct hex_map_t
    {
        ui_polygon_t hexagon;

        data::hex_grid_t* hex_grid = nullptr;

        hex_map_t();

        void render_grid_overlay();
    };
}
}
}