#include "minimap.h"

#pragma once

#include <glm/glm.hpp>

#include "asdf_multiplat/data/gl_vertex_spec.h"

#include "ui/hex_map.h"

namespace asdf
{
namespace hexmap
{
namespace ui
{
    gl_vertex_spec_<vertex_attrib::position3_t, vertex_attrib::color_t> minimap_vertex_t::vertex_spec;

    minimap_t::minimap_t(ui::hex_map_t const& _rendered_map)
    : rendered_map(_rendered_map)
    , map_data(_rendered_map.map_data);
    {
    }
}
}
}