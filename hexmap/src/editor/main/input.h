#pragma once

#include "main/hexagon_input.h"

namespace asdf {
namespace hexmap {
namespace editor
{
    struct input_handler_t : input::hex_map_input_t
    {
        using input::hex_map_input_t::hex_map_input_t;

        /*input_handler_t(ui::hex_map_t* _hm, camera_t& _c)
        : input::hex_map_input_t(_hm, _c)
        {}*/

        bool on_event(SDL_Event* event) override;
    };
}
}
}