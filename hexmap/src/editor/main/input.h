#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>

#include "main/hexagon_input.h"

namespace asdf {
namespace hexmap {
namespace editor
{

    struct editor_t;

    enum actions_e
    {
          cancel_action
        , set_tool_select
        , set_tool_terrain_paint
        , set_tool_place_object
        , set_tool_place_spline
        , num_actions
    };

    constexpr std::array<SDL_Keycode, num_actions> sdl2_input_map =
    {
          SDLK_ESCAPE
        , SDLK_v // select  (same key as photoshop)
        , SDLK_b // terrain_paint (b for brush)
        , SDLK_o // place_object
        , SDLK_s // place_spline
    };

    struct input_handler_t : input::hex_map_input_t
    {
        editor_t& editor;

        int current_tile_id = 0;
        int current_object_id = 0;

        input_handler_t(editor_t& editor);

        bool on_event(SDL_Event* event) override;

        void on_key_down(SDL_Keycode key);
    };
}
}
}