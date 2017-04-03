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
    , texture("minimap", nullptr, 1024, 1024)
    {
        GL_State->init_render_target(fbo, texture);
    }



    void minimap_t::rebuild()
    {
        auto prev_fbo = GL_State->current_framebuffer;
        GL_State->bind(fbo);

        using flags_e = hex_map_t::render_flags_e;
        uint32_t flags = flags_e::terrain;
        rendered_map.render(flags_e(flags));

        glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
        GL_State->current_framebuffer = prev_fbo;
    }

    void minimap_t::render()
    {
        //render texture to quad
        auto const& quad = app.renderer->quad;
        auto& shader = app.renderer->screen_shader;

        GL_State->bind(shader);

        shader->world_matrix = mat4();
        shader->view_matrix = mat4();

        float halfwidth = app.settings.resolution_width / 2.0f;
        float halfheight = app.settings.resolution_height / 2.0f;
        shader->projection_matrix = ortho<float>(-halfwidth, halfwidth,
                            -halfheight, halfheight,
                            -1000, 1000);
        shader->update_wvp_uniform();

        glBindTexture(GL_TEXTURE_2D, texture.texture_id);
        quad.render();

        //TODO: render box-outline representing current viewport
        //      ie: how much of the map is on screen based on zoom/pan
    }
}
}
}