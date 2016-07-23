#pragma once

#include "main/hexagon_input.h"

namespace asdf {
namespace hexmap {
namespace editor
{
    struct input_handler_t : input::hex_map_input_t
    {
        int current_tile_id = 0;

        using input::hex_map_input_t::hex_map_input_t;

        bool on_event(SDL_Event* event) override;

        void on_key_down(SDL_Keycode key);
    };
}
}
}