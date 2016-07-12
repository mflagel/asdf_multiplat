#pragma once

#include <array>

#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <glm/glm.hpp>

namespace asdf
{
namespace input
{
    template <typename T>
    struct input_controller_
    {
        T input_handler;

        glm::vec3 position = glm::vec3(0.0f);
        float speed = 10.f;

        glm::vec3 direction() const { return input_handler.direction; }

        void update(float dt)
        {
            position += direction() * speed * dt;
        }

        void on_event(SDL_Event* event)
        {
            input_handler.on_event(event);
        }
    };


    enum direction_inputs_e
    {
          up
        , down
        , left
        , right
        , forward
        , backward
        , num_direction_inputs
    };

    constexpr std::array<SDL_Keycode, num_direction_inputs> sdl2_input_map =
    {
          SDLK_UP
        , SDLK_DOWN
        , SDLK_LEFT
        , SDLK_RIGHT
        , SDLK_RCTRL
        , SDLK_RSHIFT
    };

    struct sdl2_keyboard_handler_t
    {
        std::array<bool, num_direction_inputs> input_state;
        glm::vec3 direction;

        void on_input_changed();

        void on_event(SDL_Event* event);
    };

    using input_handler_sdl2_t = input_controller_<sdl2_keyboard_handler_t>;
}
}