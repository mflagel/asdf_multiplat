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


    //const glm::vec4 grid_color(0.0f, 0.0f, 0.0f, 1.0f);
    const glm::vec4 grid_color(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr float grid_overlay_thickness = 1.0f;


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

        hex_gl_data.set_data(hex_grid.chunks[0][0]);
        
        ASSERT(!CheckGLError(), "");
        GL_State.bind(hexagons_vao);

            GL_State.bind(hexagon.vbo);
            polygon_vertex_t::vertex_spec.set_vertex_attribs(shader);


            GL_State.bind(hex_gl_data);
            GLint attrib_loc = glGetAttribLocation(shader->shader_program_id, "TileID");
            glEnableVertexAttribArray(attrib_loc); //tell the location
            glVertexAttribPointer(attrib_loc, 1, GL_UNSIGNED_INT, false, sizeof(data::hex_tile_id_t), 0);
            glVertexAttribDivisor(attrib_loc, 1); //second arg is 1, which means the vertex buffer for hexagon tile ID is incremented every instance, instead of every vertex

        GL_State.unbind_vao();

        ASSERT(!CheckGLError(), "");

        std::array<glm::vec4, num_tile_colors> colors =
        {
              COLOR_RED
            , COLOR_GREEN
            , COLOR_BLUE
            , COLOR_CYAN
            , COLOR_YELLOW
            , COLOR_MAGENTA
            , COLOR_TEAL
            , COLOR_ORANGE
            , COLOR_LIGHTGREY
            , COLOR_GREY
        };

        set_tile_colors(colors);
        ASSERT(!CheckGLError(), "");
    }

    void hex_map_t::set_tile_colors(std::array<glm::vec4, num_tile_colors> const& colors)
    {
        GL_State.bind(shader);
        glUniform4fv(shader->uniform("TileColors[0]"), num_tile_colors, (GLfloat*)colors.data());
        ASSERT(!CheckGLError(), "");
    }


    void hex_map_t::render()
    {
        //test
        ASSERT(hex_grid.chunks.size(), "");
        ASSERT(hex_grid.chunks[0].size(), "");
        render_chunk(hex_grid.chunks[0][0]);

        glLineWidth(grid_overlay_thickness);
        render_grid_overlay(hex_grid.size);
    }

    void hex_map_t::render_chunk(data::hex_grid_chunk_t const& chunk)
    {
        GL_State.bind(shader);
        GL_State.bind(hexagons_vao);
        render_hexagons(glm::ivec2(data::chunk_width, data::chunk_height), GL_TRIANGLE_FAN);
        GL_State.unbind_vao();
    }

    void hex_map_t::render_grid_overlay(glm::ivec2 grid_size)
    {
        GL_State.bind(shader);
        GL_State.bind(hexagon.vao);
        render_hexagons(grid_size, hexagon.draw_mode);
        GL_State.unbind_vao();
    }

    void hex_map_t::render_hexagons(glm::ivec2 grid_size, GLuint draw_mode)
    {
        for(size_t x = 0; x < grid_size.x; ++x)
        {
            bool even_col = x%2 == 0;
            shader->world_matrix[3][0] = hex_width_d4 * 3 * x;
            shader->world_matrix[3][1] = !even_col * -hex_height_d2;

            shader->update_wvp_uniform();

            size_t n = (size_t)(grid_size.y);

            glDrawArraysInstanced(draw_mode, 0, 6, n); //start at 0th, draw 6 points per shape, draw (width/2)
        }
    }



    void hex_buffer_data_t::set_data(data::hex_grid_chunk_t const& chunk)
    {
        std::array<data::hex_tile_id_t, data::chunk_width * data::chunk_height> cell_data;

        for(size_t y = 0; y < data::chunk_height; ++y)
        {
            for(size_t x = 0; x < data::chunk_width; ++x)
            {
                //cell_data[y*data::chunk_height + x] = chunk.cells[x][y].tile_id;
                cell_data[y*data::chunk_height + x] = rand() % 10;
                //cell_data[y*data::chunk_height + x] = 1;// + rand() % 3;
            }
        }

        GL_State.bind(*this);
        GL_State.buffer_data(*this, (cell_data.size() * sizeof(data::hex_tile_id_t)), (GLvoid*)(cell_data.data()) );

        ASSERT(!CheckGLError(), "Error setting hexagon buffer data");
    }
}
}
}