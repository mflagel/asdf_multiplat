#pragma once

#include "stdafx.h"

namespace asdf
{
    struct viewport_t
    {
        glm::vec2 bottom_left;
        glm::vec2 size_d2;
    };

    struct camera_t
    {
        glm::vec3 position;
        glm::vec3 direction = glm::vec3{0.0f, 0.0f, -1.0f};
        viewport_t viewport;

        float near_plane = -1000;
        float  far_plane =  1000;

        float zoom() const { return position.z * position.z; }

        glm::mat4 view_matrix() const;
        glm::mat4 projection_ortho() const;
        glm::mat4 projection_ortho(glm::vec2 viewport_d2) const;

        glm::vec3 screen_to_world_coord(glm::vec2 const& screen_coord) const;
    };
}