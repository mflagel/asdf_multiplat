#include "minimap.h"

#include <glm/glm.hpp>

#include "asdf_multiplat/data/gl_vertex_spec.h"
#include "asdf_multiplat/data/gl_state.h"

#include "ui/hex_map.h"

namespace asdf
{
namespace hexmap
{
namespace ui
{
    gl_vertex_spec_<vertex_attrib::position2_t
                  , vertex_attrib::color_t> minimap_vertex_t::vertex_spec;

    minimap_t::minimap_t(ui::hex_map_t const& _rendered_map)
    : rendered_map(_rendered_map)
    , map_data(_rendered_map.map_data)
    {
        GL_State->init_render_target(fbo, texture);
    }



    void minimap_t::render()
    {
        auto prev_fbo = GL_State->current_framebuffer;
        GL_State->bind(fbo);

        using flags_e = hex_map_t::render_flags_e;
        uint32_t flags = flags_e::terrain;
        //rendered_map.render(flags_e(flags));

        glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
        GL_State->current_framebuffer = prev_fbo;
    }
}
}
}