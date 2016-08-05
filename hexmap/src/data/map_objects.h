#pragma once

#include <vector>

#include <glm/glm.h">

#include "asdf_multiplat/main/asdf_defs.h"

namespace asdf {
namespace hexmap {
namespace data
{
    struct map_object_t
    {
        uint64_t id;

        glm::vec3 position;
    };

    struct spline_t
    {

    };

    struct map_objects_t
    {
        std::vector<map_object_t> objects;
        std::vector<spline_t> splines;
    };
}
}
}
