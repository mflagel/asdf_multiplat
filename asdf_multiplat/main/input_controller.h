#pragma once

#include <array>

#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <glm/glm.hpp>

#include "main/mouse.h"
#include "main/keyboard.h"

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

    using input_handler_sdl2_t = input_controller_<sdl2_keyboard_handler_t>;
}
}