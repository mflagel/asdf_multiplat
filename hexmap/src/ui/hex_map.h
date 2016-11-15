#pragma once

#include <glm/glm.hpp>


#include "asdf_multiplat/main/input_controller.h"
#include "asdf_multiplat/ui/ui_base.h"
#include "asdf_multiplat/data/texture_atlas.h"
#include "asdf_multiplat/utilities/spritebatch.h"

#include "data/hex_grid.h"
#include "data/map_objects.h"
#include "data/terrain_bank.h"

//for whatever reason Qt requires this, but libhexmap.mk does not somehow
#include "asdf_multiplat/main/input_controller.h"
#include "asdf_multiplat/ui/polygon.h"
#include "asdf_multiplat/utilities/camera.h"
#include "data/hex_map.h"

using color_t = glm::vec4;

struct cJSON;

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

    constexpr float hex_edge_length = hex_width * 0.52359877559f;  //width * sin(pi/6)

    constexpr float px_per_unit = 128.0f;
    constexpr float units_per_px = 1.0f / px_per_unit;

    constexpr std::array<float, 18> hexagon_points =
    {
           hex_width_d2,   0.0f,           0.0f   // mid right
        ,  hex_width_d4,  -hex_height_d2,  0.0f   // bottom right
        , -hex_width_d4,  -hex_height_d2,  0.0f   // bottom left
        , -hex_width_d2,   0.0f,           0.0f   // middle left
        , -hex_width_d4,   hex_height_d2,  0.0f   // top left
        ,  hex_width_d4,   hex_height_d2,  0.0f   // top right
    };

namespace ui
{
    constexpr size_t num_tile_colors = 10;


    struct  hexagon_vertex_t
    {
        static gl_vertex_spec_<vertex_attrib::position3_t/*, vertex_attrib::color_t*/> vertex_spec;

        glm::vec3   position;
        //glm::vec4   color;
    };


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
        data::hex_map_t& map_data;

        asdf::camera_t camera;
        asdf::input_handler_sdl2_t camera_controller;

        std::shared_ptr<shader_t> shader;
        rendered_polygon_<hexagon_vertex_t> hexagon;

        data::terrain_bank_t terrain_bank;
        std::unique_ptr<asdf::data::texture_atlas_t> ojects_atlas;

        vao_t hexagons_vao; //used for instancing the hex tiles
        hex_buffer_data_t hex_gl_data;
        spritebatch_t spritebatch; //used to render map objects

        
        hex_map_t(data::hex_map_t& _map_data);

        void set_tile_colors(std::array<glm::vec4, num_tile_colors> const&);

        void update(float dt);
        void render();
        void on_event(SDL_Event* event);

        void render_chunk(data::hex_grid_chunk_t const&);
        void render_grid_overlay(glm::ivec2 grid_size);
        void render_hexagons(glm::ivec2 grid_size, GLuint draw_mode);
        void render_map_objects();
    };
}
}
}
