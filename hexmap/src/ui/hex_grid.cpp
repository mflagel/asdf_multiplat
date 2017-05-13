#include "stdafx.h"
#include "hex_map.h"

#include <array>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "asdf_multiplat/main/asdf_multiplat.h"
#include "asdf_multiplat/data/gl_resources.h"
#include "asdf_multiplat/data/content_manager.h"

using namespace std;
using namespace glm;

namespace asdf
{
    using namespace util;
    using namespace data;
    
namespace hexmap
{
namespace ui
{
    using render_flags_e = hex_map_t::render_flags_e;

    const glm::vec4 grid_color(0.0f, 0.0f, 0.0f, 1.0f);
    // const glm::vec4 grid_color(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr float grid_overlay_thickness = 2.0f;

    //constexpr char terrain_types_json_filename[] = "terrain_types.json";

    //constexpr int apply_hexagon_textures = 1;


    gl_vertex_spec_<vertex_attrib::position3_t/*, vertex_attrib::color_t*/> hexagon_vertex_t::vertex_spec;



    hex_grid_t::hex_grid_t(data::hex_grid_t& _grid_data)
    : grid_data(_grid_data)
    {
        are_hexagons_instanced = GLEW_VERSION_3_3;

        shader = Content.create_shader_highest_supported("hexmap");


        std::vector<hexagon_vertex_t> verts(6);

        for(size_t i = 0; i < 6; ++i)
        {
            verts[i].position.x = hexagon_points[i*3 + 0];
            verts[i].position.y = hexagon_points[i*3 + 1];
            verts[i].position.z = hexagon_points[i*3 + 2];

            //verts[i].color = grid_color;
        }

        hexagon.initialize(shader);
        hexagon.draw_mode = GL_LINE_LOOP;


        if(are_hexagons_instanced)
        {
            hexagon.set_data(verts);
        }
        else
        {
            ///FIXME figure out how to deal with chunks of differing sizes
            //for now just throw a bunch of vertices at the wall
            //lazy: just fill a buffer with enough copies of hexagon verts
            //      that it will fill a chunk
            auto size = grid_data.chunk_size();
            std::vector<hexagon_vertex_t> non_instanced_verts;
            non_instanced_verts.reserve(size.x * size.y * verts.size());

            for(size_t i = 0; i < size.x * size.y; ++i)
            {
                //multi_draw metadata
                hexagon.first_vert_indices.push_back(non_instanced_verts.size());
                hexagon.vert_counts.push_back(6);

                non_instanced_verts.insert(non_instanced_verts.end()
                                         , verts.begin(), verts.end());
            }

            //use base class set_data() to avoid overwriting first_vert_indices and counts
            hexagon.rendered_polygon_<hexagon_vertex_t>::set_data(non_instanced_verts);
        }
        
        ASSERT(!CheckGLError(), "");
        GL_State->bind(hexagons_vao);

            GL_State->bind(hexagon.vbo);
            hexagon_vertex_t::vertex_spec.set_vertex_attribs(shader);

            GL_State->bind(hex_gl_data);
            GLint attrib_loc = glGetAttribLocation(shader->shader_program_id, "TileID");
            glEnableVertexAttribArray(attrib_loc); //tell the location

            glVertexAttribIPointer(attrib_loc, 1, GL_UNSIGNED_INT, sizeof(data::hex_tile_id_t), 0);  // must use Atrib ** I ** for unsigned int to be used as a uint in glsl

            if(are_hexagons_instanced)
            {
                glVertexAttribDivisor(attrib_loc, 1); //second arg is 1, which means the vertex buffer for hexagon tile ID is incremented every instance, instead of every vertex
            }

        GL_State->unbind_vao();

        ASSERT(!CheckGLError(), "GL Error setting hexmap VAO and vertex attributes");
    }


    void hex_grid_t::render_chunk(data::hex_grid_chunk_t const& chunk, render_flags_e render_flags)
    {
        GL_State->bind(hexagons_vao);

        if(are_hexagons_instanced)
        {
            hex_gl_data.set_data_instanced(chunk);
        }
        else
        {
            hex_gl_data.set_data(chunk);
        }

        float chunk_width_cells = hex_width_d4 * 3 * chunk.allocated_size.x;
        float chunk_height_cells = hex_height * chunk.allocated_size.y;

        shader->world_matrix[3][0] = chunk.position.x * chunk_width_cells;
        shader->world_matrix[3][1] = chunk.position.y * chunk_height_cells;
        shader->update_wvp_uniform();

        glUniform4f(shader->uniform("Color"), 1.0f, 1.0f, 1.0f, 1.0f);

        if(terrain_bank && (render_flags & render_flags_e::terrain) > 0)
        {
            glBindTexture(GL_TEXTURE_2D, terrain_bank->atlas_texture.texture_id);
            render_hexagons(chunk.size, GL_TRIANGLE_FAN);
        }

        /// if not using instanced rendering, just tack the grid draw on here
        /// since all the matricies and such are set up anyways
        if(!are_hexagons_instanced && (render_flags & render_flags_e::grid_outline) > 0)
        {
            //TODO: refactor setting of state for grid render?
            glLineWidth(grid_overlay_thickness);
            glUniform4fv( shader->uniform("Color"), 1, glm::value_ptr(grid_color) );
            glUniform1i(shader->uniform("ApplyTexture"), 0);  //turn of textures for the grid

            render_hexagons(chunk.size, GL_LINE_LOOP);

            glUniform1i(shader->uniform("ApplyTexture"), 1);  //turn textures back on for next chunk render
        }

        GL_State->unbind_vao();
    }


    void hex_grid_t::render_hexagons(glm::uvec2 grid_size, GLuint draw_mode) const
    {
        auto loc = shader->uniform("CHUNK_HEIGHT");
        glUniform1i(loc, grid_size.y);

        size_t n = grid_size.x * grid_size.y;

        if(are_hexagons_instanced)
        {
            glDrawArraysInstanced(draw_mode, 0, 6, n); //start at 0th, draw 6 points per shape, draw (width/2)
        }
        else
        {
            ASSERT(n <= hexagon.num_sub_meshes(), "trying to render more hexagons at once than currently possible");
            glMultiDrawArrays(draw_mode
                , hexagon.first_vert_indices.data()
                , hexagon.vert_counts.data()
                , n); //render n primatives instead of hexagon.num_sub_meshes()
                      //since we might not want the entire sub_mesh
        }   
    }

    //TODO: refactor setting of state for grid render?
    void hex_grid_t::render_grid_overlay_instanced(glm::uvec2 grid_size) const
    {
        GL_State->bind(shader);
        GL_State->bind(hexagon.vao);

        glLineWidth(grid_overlay_thickness);

        shader->world_matrix[3][0] = 0.0f;
        shader->world_matrix[3][1] = 0.0f;
        shader->update_wvp_uniform();

        glUniform4fv( shader->uniform("Color"), 1, glm::value_ptr(grid_color) );
        glUniform1i(shader->uniform("ApplyTexture"), 0);  //turn off textures for the grid

        render_hexagons(grid_size, GL_LINE_LOOP);
        GL_State->unbind_vao();
    }



    /// For whatever reason glVertexBindingDivisor doesn't seem to work
    /// so for compatability reasons, I'll just duplicate the tileID data
    /// so that it's per vertex instead of per-primative
    void hex_buffer_data_t::set_data(data::hex_grid_chunk_t const& chunk)
    {
        constexpr size_t num_verts = 6; //6 verts per hex
        std::vector<data::hex_tile_id_t> cell_data(chunk.size.x * chunk.size.y * num_verts);

        for(size_t x = 0; x < chunk.size.x; ++x)
        {
            for(size_t y = 0; y < chunk.size.y; ++y)
            {
                for(size_t i = 0; i < num_verts; ++i)
                {
                    //x*chunk.size.y + y gets cell
                    //multiply that by num_verts because that's how many elements per cell
                    //add i to get current element
                    size_t vert_index = (x*chunk.size.y + y) * num_verts + i;
                    cell_data[vert_index] = chunk.cells[x][y].tile_id;

                    // cell_data[vert_index] = chunk.position.x + abs(chunk.position.y);  //set id to chunk pos for debugging chunks
                }
            }
        }

        GL_State->bind(*this);
        GL_State->buffer_data(*this, (cell_data.size() * sizeof(data::hex_tile_id_t)), reinterpret_cast<GLvoid*>(cell_data.data()) );

        ASSERT(!CheckGLError(), "Error setting hexagon buffer data");
    }

    void hex_buffer_data_t::set_data_instanced(data::hex_grid_chunk_t const& chunk)
    {
        std::vector<data::hex_tile_id_t> cell_data(chunk.size.x * chunk.size.y);

        for(size_t x = 0; x < chunk.size.x; ++x)
        {
            for(size_t y = 0; y < chunk.size.y; ++y)
            {
                //cell_data[x*chunk.size.y + y] = chunk.position.x + abs(chunk.position.y);  //set id to chunk pos for debugging chunks
                cell_data[x*chunk.size.y + y] = chunk.cells[x][y].tile_id;
            }
        }

        GL_State->bind(*this);
        GL_State->buffer_data(*this, (cell_data.size() * sizeof(data::hex_tile_id_t)), reinterpret_cast<GLvoid*>(cell_data.data()) );

        ASSERT(!CheckGLError(), "Error setting hexagon buffer data");
    }
    
}
}
}
