#pragma once

#include "texture.h"

namespace asdf
{
namespace projector_fun
{

    /*
    for each pixel (ie: in frag shader)
        for each metaball
            calc metaball func


    metaball calc: M(x,y) = R / sqrt( (x-x0)^2 + (y-y0)^2 )
                        // radius / dist(p0, p1)

    */

    // this will be a software-rendered version. I'll probably write a GLSL version later
    // currently generates a texture

    struct sphere_t
    {
        glm::vec3 position;
        float radius;
    };

    struct metaball_t : sphere_t
    {
        glm::vec3 velocity = glm::vec3{0.0f};

        metaball_t(glm::vec3 _pos, float r, glm::vec3 _velocity = glm::vec3{0.0f})
        : sphere_t{_pos, r}
        , velocity{_velocity}
        {
        }

        float calc(glm::vec3 other_pos) const
        {
            return (radius / glm::distance(other_pos, position) ); 
        }
    };

    struct metaballs_t
    {
        enum draw_mode_e
        {
              hollow
            , fill
            , shaded
            , inverse
        };

        const size_t texture_width  = 500;
        const size_t texture_height = 500;

        color_t clear_color = color_t(0.02f, 0.02f, 0.02f, 1.0f);

        std::vector<metaball_t> balls;

        draw_mode_e draw_mode = hollow;
        // color_t metaball_texture[texture_height*texture_width]; //RBGA 1D texture
        color_t color_data[500*500]; //bottom left is [0,0] top right is [n-1,n-1]
        std::shared_ptr<texture_t> texture = nullptr;

        metaballs_t();

        glm::vec3 get_pos(const size_t x, const size_t y) const;
        void draw_metaballs();
        void draw_pixel(const size_t x, const size_t y, const float sum);
        std::shared_ptr<texture_t> array_to_texture();

        void update(float dt);
    };

}
}