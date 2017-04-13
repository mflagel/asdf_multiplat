#pragma once

#include "stdafx.h"

namespace asdf
{
    struct viewport_t
    {
        glm::vec2 bottom_left;
        glm::vec2 size_d2;
    };

    float zoom_for_size(viewport_t const& viewport, glm::vec2 const& size);
    viewport_t viewport_for_size_aspect(glm::vec2 const&, float aspect_ratio);

    struct camera_t
    {
        glm::vec3 position  = glm::vec3{0.0f, 0.0f, 1.0f};
        glm::vec3 direction = glm::vec3{0.0f, 0.0f, -1.0f};
        viewport_t viewport;
        float aspect_ratio = 0.0f;

        float near_plane = -1000;
        float  far_plane =  1000;

        float zoom() const { return position.z; }
        void zoom_to_size(glm::vec2 const&);

        glm::mat4 view_matrix() const;
        glm::mat4 projection_ortho() const;
        glm::mat4 projection_ortho(glm::vec2 viewport_d2) const;

        glm::vec3 screen_to_world_coord(glm::vec2 const& screen_coord) const;
    };
}