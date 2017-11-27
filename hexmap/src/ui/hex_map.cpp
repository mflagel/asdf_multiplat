#include "stdafx.h"
#include "hex_map.h"

#include <array>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <asdfm/main/asdf_multiplat.h>
#include <asdfm/data/gl_resources.h>
#include <asdfm/data/content_manager.h>

#include "data/hex_util.h"


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

    //const glm::vec4 grid_color(0.0f, 0.0f, 0.0f, 1.0f);
    // const glm::vec4 grid_color(1.0f, 1.0f, 1.0f, 1.0f);

    constexpr int apply_hexagon_textures = 1;


    hex_map_t::hex_map_t(data::hex_map_t& _map_data)
    : hex_grid_t(_map_data.hex_grid, _map_data.terrain_bank)
    , map_data(_map_data)
    {
        spritebatch.spritebatch_shader = Content.shaders["spritebatch"];

        spline_renderer.init(Content.create_shader_highest_supported("spline"));
        spline_renderer.spline_list = &map_data.splines;
    }

    void hex_map_t::update(float dt)
    {
        camera_controller.update(dt);
        camera_controller.position.z = glm::clamp(camera_controller.position.z, -16.0f, 16.0f);
        camera.position = camera_controller.position;
    }

    void hex_map_t::render(render_flags_e render_flags)
    {
        WARN_IF(camera.aspect_ratio == 0.0f, "Camera has no aspect ratio!");

        ASSERT(map_data.hex_grid.chunks.size(), "");
        ASSERT(map_data.hex_grid.chunks[0].size(), "");

        shader->world_matrix = glm::mat4();
        shader->view_matrix       = camera.view_matrix();
        shader->projection_matrix = camera.projection_ortho();
        
        GL_State->bind(shader);
        glUniform1i(shader->uniform("ApplyTexture"), apply_hexagon_textures);

        if((render_flags & render_flags_e::terrain) > 0)
        {
            map_data.hex_grid.for_each_chunk( [this, render_flags](data::hex_grid_chunk_t& chunk) -> void
            {
                render_chunk(chunk, render_flags);
            });
        }

        
        if(are_hexagons_instanced && (render_flags & render_flags_e::grid_outline) > 0)
        {
            render_grid_overlay_instanced(map_data.hex_grid.size);
        }
        
        if((render_flags & render_flags_e::map_objects) > 0)
        {
            render_map_objects();
        }

        if((render_flags & render_flags_e::splines) > 0)
        {
            spline_renderer.rebuild_if_dirty();
            render_splines();
        }
    }

    void hex_map_t::on_event(SDL_Event* event)
    {
        camera_controller.on_event(event);
        camera.position = camera_controller.position;
    }

    void hex_map_t::render_map_objects()
    {
        spritebatch.begin(shader->view_matrix, shader->projection_matrix);

        auto const* objects_atlas = map_data.objects_atlas.get();

        for(auto& obj : map_data.objects)
        {
            ASSERT(obj.id < objects_atlas->atlas_entries.size(), "object ID does not exist in atlas");
            auto const& atlas_entry = objects_atlas->atlas_entries[obj.id];

            // set y to the texture height minus (entry height + entry top_left), since in openGL the 0,0 coord is
            // the bottom left but in the atlas 0,0 is the top left
            rect_t src_rect(atlas_entry.top_left_px.x, objects_atlas->atlas_texture->height - (atlas_entry.top_left_px.y + atlas_entry.size_px.y),
                            atlas_entry.size_px.x, atlas_entry.size_px.y);
            
            auto sprite_size = glm::vec2(atlas_entry.size_px) * units_per_px;
            auto size_scale = (obj.size_d2 * 2.0f) / sprite_size;
            auto sprite_scale = obj.scale * size_scale * glm::vec2(units_per_px);

            spritebatch.draw(objects_atlas->atlas_texture, obj.position, src_rect, obj.color, sprite_scale, obj.rotation);
        }

        spritebatch.end();
    }

    void hex_map_t::render_splines() const
    {
        ASSERT(spline_renderer.shader, "spline shader required to render splines");

        auto& sr_s = spline_renderer.shader;
        sr_s->view_matrix       = shader->view_matrix;
        sr_s->projection_matrix = shader->projection_matrix;

        spline_renderer.render();
    }
}
}
}
