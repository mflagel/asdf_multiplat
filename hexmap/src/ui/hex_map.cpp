#include "stdafx.h"
#include "hex_map.h"

#include <array>

#include "asdf_multiplat/data/gl_resources.h"
#include "asdf_multiplat/data/content_manager.h"

namespace asdf
{
namespace hexmap
{
namespace ui
{

    constexpr float hex_width    = 1.0f;
    constexpr float hex_width_d2 = hex_width / 2.0f;
    constexpr float hex_width_d4 = hex_width_d2 / 2.0f;

    constexpr float hex_height    = 0.86602540378f; //sin(pi/3)
    constexpr float hex_height_d2 = hex_height / 2.0f;
    constexpr float hex_height_d4 = hex_height_d2 / 2.0f;


    constexpr std::array<float, 18> hexagon_points =
    {
           hex_width_d2,   0.0f,            0.0f   // mid right
        ,  hex_width_d4,  -hex_height_d2,  0.0f   // bottom right
        , -hex_width_d4,  -hex_height_d2,  0.0f   // bottom left
        , -hex_width_d2,   0.0f,            0.0f   // middle left
        , -hex_width_d4,   hex_height_d2,  0.0f   // top left
        ,  hex_width_d4,   hex_height_d2,  0.0f   // top right
    };

    const glm::vec4 grid_color(0.0f, 0.0f, 0.0f, 1.0f);


    hex_map_t::hex_map_t(data::hex_grid_t const& _hex_grid)
    : hex_grid(_hex_grid)
    {

        shader = Content.shaders["hexmap"];

        polygon_t verts(6);

        for(size_t i = 0; i < 6; ++i)
        {
            verts[i].position.x = hexagon_points[i*3 + 0];
            verts[i].position.y = hexagon_points[i*3 + 1];
            verts[i].position.z = hexagon_points[i*3 + 2];

            verts[i].color = grid_color;
        }

        hexagon.set_data(verts, shader);
        hexagon.draw_mode = GL_LINE_LOOP;
    }

    void hex_map_t::render()
    {
        render_grid_overlay(hex_grid.size);
    }

    void hex_map_t::render_grid_overlay(glm::ivec2 grid_size)
    {
        GL_State.bind(shader);

        GL_State.bind(hexagon.vao);

        for(size_t y = 0; y < grid_size.y * 2; ++y)  //use size.y * 2, since each loop iteration increments a half-height
        {
            bool even_row = (y%2 == 0);
            shader->world_matrix[3][0] = !even_row * hex_width_d4 * 3;  //offset horizontally for odd y positions
            shader->world_matrix[3][1] = -hex_height_d2 * y;  //height offset

            shader->update_wvp_uniform();

            //if an odd width, draw an extra hex every even row
            size_t n = std::floor(grid_size.x / 2.0f);
            n += ((grid_size.x % 2 == 1) * even_row);

            glDrawArraysInstanced(hexagon.draw_mode, 0, 6, n); //start at 0th, draw 6 points per shape, draw (width/2)
        }

        GL_State.unbind_vao();
    }
}
}
}