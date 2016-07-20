#pragma once

#include <glm/glm.hpp>

#include "asdf_multiplat/ui/polygon.h"
#include "asdf_multiplat/ui/ui_base.h"

#include "data/hex_grid.h"
#include "main/hexagon_input.h"


using color_t = glm::vec4;

namespace asdf
{
namespace hexmap
{
namespace ui
{
    constexpr size_t num_tile_colors = 10;


    struct hex_buffer_data_t : vbo_t //ubo_t  switching to vbo and using glVertexAttribDivisor
    {
        hex_buffer_data_t()
        {
            usage = GL_STREAM_DRAW;  //using stream since I change data every frame. Will probably optimize to setting once later
        }

        hex_buffer_data_t(data::hex_grid_chunk_t const& _chunk)
        : hex_buffer_data_t()
        {
            set_data(_chunk);
        }

        void set_data(data::hex_grid_chunk_t const&);
    };


    struct hex_map_t
    {
        data::hex_grid_t& hex_grid;

        std::shared_ptr<shader_t> shader;
        rendered_polygon_<polygon_vertex_t> hexagon;

        vao_t hexagons_vao; //used for instancing the hex tiles
        hex_buffer_data_t hex_gl_data;

        
        hex_map_t(data::hex_grid_t& hex_grid);

        void set_tile_colors(std::array<glm::vec4, num_tile_colors> const&);

        void render();

        void render_chunk(data::hex_grid_chunk_t const&);
        void render_grid_overlay(glm::ivec2 grid_size);
        void render_hexagons(glm::ivec2 grid_size, GLuint draw_mode);
    };
}
}
}