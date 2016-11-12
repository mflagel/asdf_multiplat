#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>

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

    struct input_handler_t : thing_that_gets_events_t
    {
        editor_t& editor;

        uint32_t modifier_keys = 0;

        input_handler_t(editor_t& editor);

        glm::vec2 world_coords(glm::ivec2 screen_coords);

        bool on_mouse_down(mouse_button_event_t&) override;
        bool on_mouse_up(mouse_button_event_t&) override;
        bool on_mouse_move(mouse_motion_event_t&) override;
        bool on_mouse_wheel(mouse_wheel_event_t&) override;

        void on_key_down(SDL_Keysym keysm);
        void on_key_up(SDL_Keysym keysm);

        bool handle_click_selection(mouse_button_event_t&, glm::vec2 const& mw);
    };
}
}
}