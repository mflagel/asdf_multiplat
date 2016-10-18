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
    };

    struct spline_t
    {

    };
}
}
}
