#pragma once

#include <glm/glm.hpp>

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
        std::shared_ptr<shader_t> shader;

        rendered_polygon_<polygon_vertex_t> hexagon;

        data::hex_grid_t const& hex_grid;

        hex_map_t(data::hex_grid_t const& hex_grid);


        void render();
        void render_grid_overlay(glm::ivec2 grid_size);
    };
}
}
}