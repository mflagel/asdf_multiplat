#include "stdafx.h"
#include "hex_map.h"

#include <array>

#include "asdf_multiplat/data/gl_resources.h"

namespace asdf
{
namespace hexmap
{
namespace ui
{
    constexpr float hex_halfheight = 0.86602540378f; //sin(pi/3)
    constexpr std::array<float, 18> hexagon_points =
    {
           1.0f,   0.0f,            0.0f   // mid right
        ,  0.5f,  -hex_halfheight,  0.0f   // bottom right
        , -0.5f,  -hex_halfheight,  0.0f   // bottom left
        , -1.0f,   0.0f,            0.0f   // middle left
        , -0.5f,   hex_halfheight,  0.0f   // top left
        ,  0.5f,   hex_halfheight,  0.0f   // top right
    };

    const glm::vec4 grid_color(0.0f, 0.0f, 0.0f, 1.0f);


    hex_map_t::hex_map_t()
    {
        polygon_t verts(6);

        for(size_t i = 0; i < 6; ++i)
        {
            verts[i].position.x = hexagon_points[i*3 + 0] * 100;
            verts[i].position.y = hexagon_points[i*3 + 1] * 100;
            verts[i].position.z = hexagon_points[i*3 + 2] * 100;

            verts[i].color = grid_color;
        }

        hexagon.set_data(verts);
        hexagon.draw_mode = GL_LINE_LOOP;
    }

    void hex_map_t::render_grid_overlay()
    {
        //test
        //hexagon.render();

        GL_State.bind(hexagon.vao);
        glDrawArraysInstanced(hexagon.draw_mode, 0, 6, 10);
        GL_State.unbind_vao();
    }
}
}
}