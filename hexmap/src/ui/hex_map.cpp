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
    //const glm::vec4 grid_color(0.0f, 0.0f, 0.0f, 1.0f);
    const glm::vec4 grid_color(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr float grid_overlay_thickness = 2.0f;

    constexpr char terrain_types_json_filename[] = "terrain_types.json";

    constexpr int apply_hexagon_textures = 1;


    gl_vertex_spec_<vertex_attrib::position3_t/*, vertex_attrib::color_t*/> hexagon_vertex_t::vertex_spec;


    hex_map_t::hex_map_t(data::hex_map_t& _map_data)
    : map_data(_map_data)
    {
        shader = Content.create_shader("hexmap", 330);
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
        hexagon.set_data(verts);
        hexagon.draw_mode = GL_LINE_LOOP;
        
        ASSERT(!CheckGLError(), "");
        GL_State->bind(hexagons_vao);

            GL_State->bind(hexagon.vbo);
            hexagon_vertex_t::vertex_spec.set_vertex_attribs(shader);
            ASSERT(!CheckGLError(), "Error setting vertex attributes");

            GL_State->bind(hex_gl_data);
            GLint attrib_loc = glGetAttribLocation(shader->shader_program_id, "TileID");
            glEnableVertexAttribArray(attrib_loc); //tell the location
            glVertexAttribIPointer(attrib_loc, 1, GL_UNSIGNED_INT, sizeof(data::hex_tile_id_t), 0);  // must use Atrib ** I ** for unsigned int to be used as a uint in glsl
            glVertexAttribDivisor(attrib_loc, 1); //second arg is 1, which means the vertex buffer for hexagon tile ID is incremented every instance, instead of every vertex

        GL_State->unbind_vao();

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


        auto dir = find_folder("data");
        ASSERT(dir.length() > 0, "Could not find data folder");

        auto terrain_types_json_filepath = dir + "/" + string(terrain_types_json_filename);
        terrain_bank.load_from_file(terrain_types_json_filepath);
        

        objects_atlas = make_unique<texture_atlas_t>(string(dir + "/../assets/Objects/objects_atlas_data.json"));

        
        spline_renderer.init(Content.create_shader("spline", "colored", 330));
    }

    void hex_map_t::set_tile_colors(std::array<glm::vec4, num_tile_colors> const& colors)
    {
        GL_State->bind(shader);
        glUniform4fv(shader->uniform("TileColors[0]"), num_tile_colors, reinterpret_cast<const GLfloat*>(colors.data()));
        ASSERT(!CheckGLError(), "");
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

        
        glUniform1i(shader->uniform("ApplyTexture"), 0);  //turn of textures for the grid
        render_grid_overlay(map_data.hex_grid.size);
        

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

        hex_gl_data.set_data(chunk);

        float chunk_width_cells = hex_width_d4 * 3 * chunk.allocated_size.x;
        float chunk_height_cells = hex_height * chunk.allocated_size.y;

        shader->world_matrix[3][0] = chunk.position.x * chunk_width_cells;
        shader->world_matrix[3][1] = chunk.position.y * chunk_height_cells;
        shader->update_wvp_uniform();

        glUniform4f(shader->uniform("Color"), 1.0f, 1.0f, 1.0f, 1.0f);

        glBindTexture(GL_TEXTURE_2D, terrain_bank.atlas_texture.texture_id);

        render_hexagons(chunk.size, GL_TRIANGLE_FAN);
        GL_State->unbind_vao();
    }

    void hex_map_t::render_grid_overlay(glm::uvec2 grid_size)
    {
        GL_State->bind(shader);
        GL_State->bind(hexagon.vao);

        glLineWidth(grid_overlay_thickness);

        shader->world_matrix[3][0] = 0.0f;
        shader->world_matrix[3][1] = 0.0f;
        shader->update_wvp_uniform();

        glUniform4fv( shader->uniform("Color"), 1, glm::value_ptr(grid_color) );

        render_hexagons(grid_size, hexagon.draw_mode);
        GL_State->unbind_vao();
    }

    void hex_map_t::render_hexagons(glm::uvec2 grid_size, GLuint draw_mode)
    {
        auto loc = shader->uniform("CHUNK_HEIGHT");
        glUniform1i(loc, grid_size.y);

        size_t n = grid_size.x * grid_size.y;
        glDrawArraysInstanced(draw_mode, 0, 6, n); //start at 0th, draw 6 points per shape, draw (width/2)
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

        spline_renderer.spline_batch.clear(); //temp
        spline_renderer.batch(map_data.splines);
        spline_renderer.end();


        //TODO: draw nodes and control nodes on selected splines
        //      especially the WIP spline if there is one
    }



    void hex_buffer_data_t::set_data(data::hex_grid_chunk_t const& chunk)
    {
        //std::array<data::hex_tile_id_t, data::max_chunk_width * data::max_chunk_height> cell_data;
        std::vector<data::hex_tile_id_t> cell_data(chunk.size.x * chunk.size.y);

        for(size_t x = 0; x < chunk.size.x; ++x)
        {
            for(size_t y = 0; y < chunk.size.y; ++y)
            {
                //cell_data[x*chunk.size.y + y] = rand() % 10;
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