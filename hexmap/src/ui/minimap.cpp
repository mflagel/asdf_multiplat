#include "minimap.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "asdf_multiplat/main/asdf_multiplat.h"
#include "asdf_multiplat/data/gl_vertex_spec.h"
#include "asdf_multiplat/data/gl_state.h"

#include "ui/hex_map.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace hexmap
{
namespace ui
{
    gl_vertex_spec_<vertex_attrib::position2_t
                  , vertex_attrib::color_t> minimap_vertex_t::vertex_spec;

    minimap_t::minimap_t(ui::hex_map_t& _rendered_map)
    : rendered_map(_rendered_map)
    , map_data(_rendered_map.map_data)
    , render_target(1024, 1024)
    {
        render_target.texture.name = "minimap";
        render_target.init();
    }



    void minimap_t::rebuild()
    {
        scoped_render_target_t scoped(this->render_target);  //MSVC is making me put a this->  why?

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use glBlendFuncSeparate to allow the minimap FBO to have a transparent background
        // instead of black
        //glEnable(GL_BLEND);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        //snapshot the camera
        auto& camera = rendered_map.camera;
        auto prev_camera = camera;

        //set camera such that the entire map will fit within the fbo when rendered
        auto grid_size = map_data.hex_grid.size_units();
        auto grid_size_d2 = grid_size / 2.0f;
        auto grid_bbox = map_data.hex_grid.bounding_box_units();

        camera.position = vec3(grid_bbox.lower + grid_size_d2, 0.0f);
        camera.viewport = viewport_for_size_aspect(grid_size, 1.0f);

        //render just the terrain (and grid outline for now)
        using flags_e = hex_map_t::render_flags_e;
        uint32_t flags = flags_e::terrain | flags_e::grid_outline;
        rendered_map.render(flags_e(flags));

        camera = prev_camera;
    }

    void minimap_t::render()
    {
        //render texture to quad
        auto const& quad = app.renderer->quad;
        auto& shader = app.renderer->screen_shader;

        GL_State->bind(shader);

        float minimap_scale_px = 400.0f;

        shader->world_matrix = scale(translate(mat4(), vec3(0, 0, 0)), vec3(minimap_scale_px));
        shader->view_matrix = mat4();

        float halfwidth = app.surface_width / 2.0f;
        float halfheight = app.surface_height / 2.0f;
        // float const& halfwidth = texture.halfwidth;
        // float const& halfheight = texture.halfheight;

        //project such that each unit is one pixel
        shader->projection_matrix = ortho<float>(-halfwidth, halfwidth,
                            -halfheight, halfheight,
                            -1000, 1000);

        shader->update_wvp_uniform();

        glBindTexture(GL_TEXTURE_2D, render_target.texture.texture_id);
        quad.render();

        //TODO: render box-outline representing current viewport
        //      ie: how much of the map is on screen based on zoom/pan
    }
}
}
}