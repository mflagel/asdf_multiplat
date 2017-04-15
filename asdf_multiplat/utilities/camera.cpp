#include "stdafx.h"
#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "main/asdf_multiplat.h"

using namespace glm;

namespace asdf
{
    mat4 camera_t::view_matrix() const
    {
        return glm::lookAt(position, position + direction, vec3{0.0f, 1.0f, 0.0f});
    }

    glm::mat4 camera_t::projection_ortho() const
    {
        return projection_ortho( viewport.size_d2 / glm::pow(2.0f, zoom()) );
    }

    mat4 camera_t::projection_ortho(glm::vec2 viewport_d2) const
    {
        return ortho<float>(-viewport_d2.x, viewport_d2.x,
                        -viewport_d2.y, viewport_d2.y,
                        near_plane, far_plane);
    }

    void camera_t::zoom_to_size(glm::vec2 const& _size)
    {
        position.z = zoom_for_size(viewport, _size);
    }

    vec3 camera_t::screen_to_world_coord(vec2 const& screen_coord) const
    {
        auto _vp = app.screen_viewport();
        glm::vec4 vp(_vp.bottom_left.x, _vp.bottom_left.y, _vp.size.x, _vp.size.y);
        return unProject(vec3(screen_coord, 0.0f), view_matrix(), projection_ortho(), vp).xyz;
    }

    float zoom_for_size(viewport_t const& viewport, glm::vec2 const& size)
    {
        auto zooms = (size / 2.0f) / viewport.size_d2;
        return glm::max(zooms.x, zooms.y);
    }

    viewport_t viewport_for_size_aspect(glm::vec2 const& size, float aspect_ratio)
    {
        vec2 sz_d2 = size / 2.0f;

        //use the larger dimension so that nothing inside of size is cut off
        if(sz_d2.x >= sz_d2.y) //if x is bigger, set y
        {
            sz_d2.y = sz_d2.x / aspect_ratio;
        }
        else //if y is bigger, set x
        {
            sz_d2.x = sz_d2.y * aspect_ratio;
        }

        return viewport_t{-sz_d2, sz_d2};
    }
}