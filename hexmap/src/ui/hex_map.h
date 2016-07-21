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

    constexpr float hex_width    = 1.0f;
    constexpr float hex_width_d2 = hex_width / 2.0f;
    constexpr float hex_width_d4 = hex_width_d2 / 2.0f;

    constexpr float hex_height    = 0.86602540378f; //sin(pi/3)
    constexpr float hex_height_d2 = hex_height / 2.0f;
    constexpr float hex_height_d4 = hex_height_d2 / 2.0f;

    constexpr float hex_edge_length = hex_width * 0.52359877559;  //width * sin(pi/6)

    constexpr std::array<float, 18> hexagon_points =
    {
           hex_width_d2,   0.0f,            0.0f   // mid right
        ,  hex_width_d4,  -hex_height_d2,  0.0f   // bottom right
        , -hex_width_d4,  -hex_height_d2,  0.0f   // bottom left
        , -hex_width_d2,   0.0f,            0.0f   // middle left
        , -hex_width_d4,   hex_height_d2,  0.0f   // top left
        ,  hex_width_d4,   hex_height_d2,  0.0f   // top right
    };

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

        camera_t camera;
        asdf::input::input_handler_sdl2_t camera_controller;
        input::hex_map_input_t test_input;

        std::shared_ptr<shader_t> shader;
        rendered_polygon_<polygon_vertex_t> hexagon;

        vao_t hexagons_vao; //used for instancing the hex tiles
        hex_buffer_data_t hex_gl_data;

        
        hex_map_t(data::hex_grid_t& hex_grid);

        void set_tile_colors(std::array<glm::vec4, num_tile_colors> const&);

        void update(float dt);
        void render();
        void on_event(SDL_Event* event);

        void render_chunk(data::hex_grid_chunk_t const&);
        void render_grid_overlay(glm::ivec2 grid_size);
        void render_hexagons(glm::ivec2 grid_size, GLuint draw_mode);
    };
}
}
}