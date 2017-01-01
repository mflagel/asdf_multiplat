#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "asdf_multiplat/main/asdf_defs.h"
#include "asdf_multiplat/ui/polygon.h"
#include "data/spline.h"

namespace asdf {
    struct shader;

namespace hexmap {
namespace ui
{
    struct spline_vertex_t
    {
        static gl_vertex_spec_<vertex_attrib::position3_t, vertex_attrib::color_t> veretx_spec;

        glm::vec3 position;
        color_t color;
    };

    struct spline_renderer_t
    {
        std::unique_ptr<shader_t> shader;
        rendered_polygon_<spline_vertex_t> rendered_data;

        std::vector<data::spline_t const*> spline_batch;

        void begin();

        void batch(data::spline_t const& spline);
        void batch(std::vector<data::spline_t> const& splines);

        void end();
    };
}
}
}
