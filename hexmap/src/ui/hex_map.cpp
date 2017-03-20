#include "stdafx.h"
#include "hex_map.h"

#include <array>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    const glm::vec4 grid_color(0.0f, 0.0f, 0.0f, 1.0f);
    // const glm::vec4 grid_color(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr float grid_overlay_thickness = 2.0f;

    constexpr char terrain_types_json_filename[] = "terrain_types.json";

    constexpr int apply_hexagon_textures = 1;


    gl_vertex_spec_<vertex_attrib::position3_t/*, vertex_attrib::color_t*/> hexagon_vertex_t::vertex_spec;


    hex_map_t::hex_map_t(data::hex_map_t& _map_data)
    : map_data(_map_data)
    {
        are_hexagons_instanced = GLEW_VERSION_3_3;

        shader = Content.create_shader_highest_supported("hexmap");

        spritebatch.spritebatch_shader = Content.shaders["spritebatch"];

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
            auto size = map_data.hex_grid.chunk_size();
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


        auto dir = find_folder("data");
        ASSERT(dir.length() > 0, "Could not find data folder");

        auto terrain_types_json_filepath = dir + "/" + string(terrain_types_json_filename);
        terrain_bank.load_from_file(terrain_types_json_filepath);
        

        objects_atlas = make_unique<texture_atlas_t>(string(dir + "/../assets/Objects/objects_atlas_data.json"));

        spline_renderer.init(Content.create_shader_highest_supported("spline"));

        spline_renderer.spline_list = &map_data.splines;
    }

    void hex_map_t::update(float dt)
    {
        camera_controller.update(dt);
        camera.position = camera_controller.position;
    }

    void hex_map_t::render()
    {
        ASSERT(map_data.hex_grid.chunks.size(), "");
        ASSERT(map_data.hex_grid.chunks[0].size(), "");

        shader->world_matrix = glm::mat4();
        shader->view_matrix       = camera.view_matrix();
        shader->projection_matrix = camera.projection_ortho();
        
        GL_State->bind(shader);
        glUniform1i(shader->uniform("ApplyTexture"), apply_hexagon_textures);

        map_data.hex_grid.for_each_chunk( [this](data::hex_grid_chunk_t& chunk) -> void
        {
            render_chunk(chunk);
        });

        
        if(are_hexagons_instanced)
        {
            render_grid_overlay_instanced(map_data.hex_grid.size);
        }
        

        render_map_objects();

        render_splines();


        //TEST
        // re-importing every frame so I can capture it with nvidia's gfx debugger
        // GL_State->bind(texture_bank.atlas_fbo);
        // glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // auto dir = find_folder("data");
        // auto imported_textures_json_filepath = dir + "/" + string(imported_textures_json_filename);
        // texture_bank.saved_textures.clear(); //reset so I'm not infinitely piling stuff on
        // texture_bank.load_from_list_file(imported_textures_json_filepath);
        // glBindTexture(GL_TEXTURE_2D, texture_bank.atlas_texture.texture_id);
        //---
    }

    void hex_map_t::on_event(SDL_Event* event)
    {
        camera_controller.on_event(event);
        camera.position = camera_controller.position;
    }



    void hex_map_t::render_chunk(data::hex_grid_chunk_t const& chunk)
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

        glBindTexture(GL_TEXTURE_2D, terrain_bank.atlas_texture.texture_id);

        render_hexagons(chunk.size, GL_TRIANGLE_FAN);

        /// if not using instanced rendering, just tack the grid draw on here
        /// since all the matricies and such are set up anyways
        if(!are_hexagons_instanced)
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

    //TODO: refactor setting of state for grid render?
    void hex_map_t::render_grid_overlay_instanced(glm::uvec2 grid_size)
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

    void hex_map_t::render_hexagons(glm::uvec2 grid_size, GLuint draw_mode)
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
            //glDrawArrays(draw_mode, 0, hexagon.num_verts);
            glMultiDrawArrays(draw_mode
                , hexagon.first_vert_indices.data()
                , hexagon.vert_counts.data()
                , n); //render n primatives instead of hexagon.num_sub_meshes()
                //, hexagon.num_sub_meshes());
        }
        
    }

    void hex_map_t::render_map_objects()
    {
        spritebatch.begin(shader->view_matrix, shader->projection_matrix);

        for(auto& obj : map_data.objects)
        {
            ASSERT(obj.id < objects_atlas->atlas_entries.size(), "object ID does not exist in atlas");
            auto const& atlas_entry = objects_atlas->atlas_entries[obj.id];

            rect_t src_rect(atlas_entry.top_left_px.x, atlas_entry.top_left_px.y, atlas_entry.size_px.x, atlas_entry.size_px.y);
            auto sprite_scale = obj.scale * glm::vec2(units_per_px);

            spritebatch.draw(objects_atlas->atlas_texture, obj.position, src_rect, obj.color, sprite_scale, obj.rotation);
        }

        spritebatch.end();
    }

    void hex_map_t::render_splines()
    {
        ASSERT(spline_renderer.shader, "spline shader required to render splines");

        auto& sr_s = spline_renderer.shader;
        sr_s->view_matrix       = shader->view_matrix;
        sr_s->projection_matrix = shader->projection_matrix;

        spline_renderer.rebuild_if_dirty();
        spline_renderer.render();
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