#pragma once

#include <glm/glm.hpp>


#include <asdfm/main/input_controller.h>
#include <asdfm/ui/ui_base.h>
#include <asdfm/data/texture_atlas.h>
#include <asdfm/utilities/spritebatch.h>

#include "data/hex_map.h"
#include "data/terrain_bank.h"
#include "ui/spline_renderer.h"

//for whatever reason Qt requires this, but libhexmap.mk does not somehow
#include <asdfm/main/input_controller.h>
#include <asdfm/ui/polygon.h>
#include <asdfm/utilities/camera.h>

using color_t = glm::vec4;

struct cJSON;

namespace asdf
{
namespace hexmap
{
namespace ui
{
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
        void set_data_instanced(data::hex_grid_chunk_t const&);
    };

    struct hex_grid_t
    {
        enum render_flags_e : uint32_t
        {
              nothing      = 0
            , terrain      = 1
            , grid_outline = 2
            , hex_coords   = 4
            , map_objects  = 8
            , splines      = 16
            , everything   = 0xFFFFFFFF
        };
        
        data::hex_grid_t& grid_data;
        data::terrain_bank_t& terrain_bank;

        std::shared_ptr<shader_t> shader;
        rendered_multi_polygon_<hexagon_vertex_t> hexagon;  //multi-polyogn because we need to track info for non-instanced rendering
        vao_t hexagons_vao; //used for instancing the hex tiles
        hex_buffer_data_t hex_gl_data;

    protected:
        bool are_hexagons_instanced = false;

    public:
        hex_grid_t(data::hex_grid_t&, data::terrain_bank_t&);

        void render_chunk(data::hex_grid_chunk_t const&, render_flags_e = everything);
        void render_hexagons(glm::uvec2 grid_size, GLuint draw_mode) const;
        void render_grid_overlay_instanced(glm::uvec2 grid_size) const;
    };
}
}
}
