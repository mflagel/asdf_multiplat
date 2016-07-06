#pragma once

#include "data/hex_grid.h"

namespace hexmap
{
namespace ui
{
    struct hex_map_t
    {
        data::hex_grid_t* hex_grid = nullptr;



        hex_map_t();

        void render_grid_overlay();
    };
}
}