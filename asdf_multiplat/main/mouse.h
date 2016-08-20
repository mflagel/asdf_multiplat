#pragma once

#include <array>

#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <glm/glm.hpp>

namespace asdf
{
namespace input
{
    enum mouse_button_e : uint8_t
    {
          mouse_left = 0
        , mouse_right
        , mouse_middle
        , mouse_4
        , mouse_5
    };

    constexpr uint32_t mouse_button_bit(mouse_button_e btn)
    {
        return 1u << static_cast<uint8_t>(btn);
    }

    struct mouse_input_t
    {
        uint8_t mouse_button_states = 0;
        glm::ivec2 mouse_position;

        virtual ~mouse_input_t() = default;

        bool mouse_button_state(mouse_button_e btn) const
        {
            return (mouse_button_states & mouse_button_bit(btn)) > 0;
        }

        virtual void on_event(SDL_Event* event) = 0;
    };

    struct sdl2_mouse_input_t : mouse_input_t
    {
        void on_event(SDL_Event* event);
    };
}
}