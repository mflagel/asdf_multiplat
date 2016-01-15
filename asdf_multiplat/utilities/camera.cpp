#include "stdafx.h"
#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace asdf
{
    const glm::vec2 movespeed{10.0f};

    mat4 camera_t::view_matrix() const
    {
        return glm::lookAt(position, position + direction, vec3{0.0f, 1.0f, 0.0f});
    }

    mat4 camera_t::projection_ortho(float const& halfwidth, float const& halfheight) const
    {
        return ortho<float>(-halfwidth, halfwidth,
                        -halfheight, halfheight,
                        near_plane, far_plane);
    }

    void camera_t::on_event(SDL_Event* event)
    {
        if(event->type == SDL_KEYDOWN)
        {
            switch(event->key.keysym.sym)
            {
                case SDLK_LEFT:
                {
                    position.x -= movespeed.x;
                    break;
                }

                case SDLK_RIGHT:
                {
                    position.x += movespeed.x;
                    break;
                }

                case SDLK_UP:
                {
                    position.y += movespeed.y;
                    break;
                }

                case SDLK_DOWN:
                {
                    position.y -= movespeed.y;
                    break;
                }
            }
        }
    }
}