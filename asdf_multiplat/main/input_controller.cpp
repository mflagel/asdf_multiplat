#include "input_controller.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace input
{
    sdl2_keyboard_handler_t::sdl2_keyboard_handler_t()
    {
        for(auto& s : input_state)
            s = false;
    }

    void sdl2_keyboard_handler_t::on_input_changed()
    {
        direction = vec3(0.0f);

        direction.x += 1.0f * input_state[right];
        direction.x -= 1.0f * input_state[left];

        direction.y += 1.0f * input_state[up];
        direction.y -= 1.0f * input_state[down];

        direction.z += 1.0f * input_state[forward];
        direction.z -= 1.0f * input_state[backward];

        if(direction != vec3(0.0f))
            direction = normalize(direction);
    }

    void sdl2_keyboard_handler_t::on_event(SDL_Event* event)
    {
        if(event->type == SDL_KEYDOWN || event->type == SDL_KEYUP)
        {
            auto key = event->key.keysym.sym;

            for(size_t i = 0; i < num_direction_inputs; ++i)
            {
                auto matches = key == sdl2_input_map[(direction_inputs_e)i];

                if(event->type == SDL_KEYDOWN)
                    input_state[i] |= matches; // key state becomes true if matches
                else if(event->type == SDL_KEYUP)
                    input_state[i] &= !matches; // key state stays the same if no match, else becomes false

            }

            on_input_changed();
        }
    }
}
}