#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "asdf_multiplat/main/asdf_defs.h"

namespace asdf {
namespace hexmap {
namespace data
{
    struct map_object_t
    {
        uint64_t id;

        glm::vec2 position;
        glm::vec2 size_d2;

        color_t color;// = glm::vec4(1.0f);
        glm::vec2 scale;// = glm::vec2(1, 1);
        float rotation;// = 0.0f;
    };

    struct spline_t
    {

    };
}
}
}
