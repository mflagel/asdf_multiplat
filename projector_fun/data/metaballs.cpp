#include "stdafx.h"
#include "metaballs.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace projector_fun
{
    metaballs_t::metaballs_t()
    {
        balls.reserve(5);

        balls.push_back(metaball_t{vec3{0.0f, 0.0f, 0.0f}, 50.0f});
        balls.push_back(metaball_t{vec3{-100.0f, 100.0f, 0.0f}, 50.0f, vec3{20.0f, -150.0f, 0.0f}});
        balls.push_back(metaball_t{vec3{280.0f, -280.0f, 0.0f}, 20.0f, vec3{100.0f, -100.0f, 0.0f}});
        balls.push_back(metaball_t{vec3{0.0f, 0.0f, 0.0f}, 10.0f, vec3{100.0f, 20.0f, 0.0f}});

        //pre-fill
        for(size_t i = 0; i < (texture_width * texture_height); ++i)
        {
            color_data[i] = clear_color;
        }

        texture = std::make_shared<texture_t>("metaball_texture", color_data, texture_width, texture_height);
    }

    vec3 metaballs_t::get_pos(const size_t x, const size_t y) const
    {
        auto halfwidth = texture_width / 2;
        auto halfheight = texture_height / 2;
        auto screenpos = texture_to_screen_space(ivec2{x, y}, ivec2{halfwidth, halfheight});
        return vec3(screenpos.x, screenpos.y, 0.0f);
    }


    void metaballs_t::draw_metaballs()
    {
        //clear
        for(size_t i = 0; i < (texture_width * texture_height); ++i)
        {
            color_data[i] = clear_color;
        }

        for(size_t y = 0; y < texture_height; ++y)
        {
            for(size_t x = 0; x < texture_width; ++x)
            {
                float sum = 0; // Value to act as a summation of all Metaballs' fields applied to this particular pixel
                for(auto const& ball : balls)
                {
                    sum += ball.calc(get_pos(x,y));
                }

                draw_pixel(x, y, sum);
            }
        }

        // LOG("max sum: %f", max_sum);

        texture->write(color_data, texture_width, texture_height);
    }

    void metaballs_t::draw_pixel(const size_t x, const size_t y, const float sum)
    {
        float min_threshold = 0.99f * draw_mode != shaded;
        float max_threshold = 1.01f;

        bool should_draw = false;

        should_draw |= (draw_mode == hollow)   && sum >= min_threshold && sum <= max_threshold;
        should_draw |= (draw_mode == fill)     && sum >= min_threshold;
        should_draw |= (draw_mode == shaded)   && sum >= min_threshold;
        should_draw |= (draw_mode == inverse)  && sum <= max_threshold;

        if(should_draw)
        {
            auto color = COLOR_WHITE;

            if(draw_mode == shaded)
            {
                color = color_data[y * texture_width + x];
                color += glm::min(1.0f, sum/5.0f) * COLOR_WHITE;
            }

            color_data[y * texture_width + x] = color;
        }
    }

    std::shared_ptr<texture_t> metaballs_t::array_to_texture()
    {
        draw_metaballs();

        return std::make_shared<texture_t>("metaball test"
                                         , color_data
                                         , texture_width, texture_height);
    }

    void metaballs_t::update(float dt)
    {
        float extra_wrap_space = 0.0f;
        for(auto const& ball : balls)
        {
            extra_wrap_space = glm::max(ball.radius, extra_wrap_space);
        }

        extra_wrap_space *= 4;

        // extra_wrap_space = glm::max(texture->halfwidth, texture->halfheight);

        auto ball_lower_bounds = glm::vec3{-texture->halfwidth - extra_wrap_space, -texture->halfheight - extra_wrap_space, 0.0f};
        auto ball_upper_bounds = glm::vec3{ texture->halfwidth + extra_wrap_space,  texture->halfheight + extra_wrap_space, 0.0f};

        for(auto& ball : balls)
        {
            // wrap around sides
            ball.position.x *= 1 - (2 * (ball.position.x > ball_upper_bounds.x  || ball.position.x < ball_lower_bounds.x));
            ball.position.y *= 1 - (2 * (ball.position.y > ball_upper_bounds.y  || ball.position.y < ball_lower_bounds.y));

            ball.position += ball.velocity * dt;
        }
    }

}
}