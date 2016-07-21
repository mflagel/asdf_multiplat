#include "stdafx.h"
#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace asdf
{
    mat4 camera_t::view_matrix() const
    {
        return glm::lookAt(position, position + direction, vec3{0.0f, 1.0f, 0.0f});
    }

    glm::mat4 camera_t::projection_ortho() const
    {
        auto zoom = position.z * position.z;
        return projection_ortho(viewport.size_d2 / zoom);
    }

    mat4 camera_t::projection_ortho(glm::vec2 viewport_d2) const
    {
        return ortho<float>(-viewport_d2.x, viewport_d2.x,
                        -viewport_d2.y, viewport_d2.y,
                        near_plane, far_plane);
    }

    vec3 camera_t::screen_to_world_coord(vec2 const& screen_coord) const
    {
        vec4 vp;
        vp.xy = viewport.bottom_left;
        vp.zw = viewport.size_d2 * 2.0f;
        return unProject(vec3(screen_coord, 0.0f), view_matrix(), projection_ortho(), vp).xyz;
    }
}