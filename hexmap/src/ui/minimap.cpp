#include "minimap.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <asdfm/main/asdf_multiplat.h>
#include <asdfm/data/gl_vertex_spec.h>
#include <asdfm/data/gl_state.h>

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
        camera.viewport = viewport_for_size_aspect(grid_size, camera.aspect_ratio);

        //render just the terrain (and grid outline for now)
        using flags_e = hex_map_t::render_flags_e;
        uint32_t flags = flags_e::terrain;
        rendered_map.render(flags_e(flags));

        camera = prev_camera;

        ASSERT(!CheckGLError(), "GL Error building minimap");
    }

    void minimap_t::render()
    {
        //render texture to quad
        auto const& quad = app.renderer->quad;
        auto& shader = app.renderer->screen_shader;

        GL_State->bind(shader);

        shader->world_matrix = mat4();
        shader->view_matrix = mat4();
        shader->projection_matrix = glm::ortho<float>(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
        shader->update_wvp_uniform();

        glBindTexture(GL_TEXTURE_2D, render_target.texture.texture_id);
        // quad.render();
        quad.render_without_vao(shader);

        //TODO: render box-outline representing current viewport
        //      ie: how much of the map is on screen based on zoom/pan
    }
}
}
}
