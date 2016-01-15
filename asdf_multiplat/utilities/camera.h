#pragma once

#include "stdafx.h"

namespace asdf
{
    struct camera_t
    {
        glm::vec3 position;
        glm::vec3 direction = glm::vec3{0.0f, 0.0f, -1.0f};

        float near_plane = -1000;
        float  far_plane =  1000;

        glm::mat4 view_matrix() const;
        glm::mat4 projection_ortho(float const& halfwidth, float const& halfheight) const;

        void on_event(SDL_Event* event);
    };
}