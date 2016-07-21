#pragma once
#pragma once

#include <array>

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <glm/glm.hpp>

namespace asdf
{
namespace input
{
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

        sdl2_keyboard_handler_t();

        void on_input_changed();

        void on_event(SDL_Event* event);
    };
}
}