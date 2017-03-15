#include "mouse.h"

namespace asdf
{
    void process_sdl_mouse_events(mouse_input_t& mouse, SDL_Event*);

    constexpr bool is_sdl_mouse_event(SDL_Event* event)
    {
        return event->type == SDL_MOUSEMOTION
            || event->type == SDL_MOUSEBUTTONDOWN
            || event->type == SDL_MOUSEBUTTONUP
            || event->type == SDL_MOUSEWHEEL
            ;
    }

    /// As much as I'd love to just cast SDL's uint8 to a mouse_button_e
    /// they rather annoyingly have mid-mouse as button 2 and right-mouse as button 3
    /// NOTE this does NOT take a flag, but rather the numbered value from 1 to 5
    /// MSVC CONSTEXPR
    /*constexpr*/inline mouse_button_e mouse_button_from_sdl_btn(uint32_t sdl_btn)
    {
        uint32_t btn = 0;
        btn += mouse_left   * (sdl_btn == SDL_BUTTON_LEFT);
        btn += mouse_right  * (sdl_btn == SDL_BUTTON_RIGHT);
        btn += mouse_middle * (sdl_btn == SDL_BUTTON_MIDDLE);
        btn += mouse_4      * (sdl_btn == SDL_BUTTON_X1);
        btn += mouse_5      * (sdl_btn == SDL_BUTTON_X2);

        ASSERT(btn < num_mouse_buttons, "");
        ASSERT(btn != 0, "");

        return mouse_button_e(btn);
    }

    mouse_button_event_t mouse_event_from_sdl(mouse_input_t&, SDL_MouseButtonEvent const&);
    mouse_motion_event_t mouse_event_from_sdl(mouse_input_t&, SDL_MouseMotionEvent const&);
    mouse_wheel_event_t  mouse_event_from_sdl(mouse_input_t&, SDL_MouseWheelEvent const&);

    uint32_t modifier_keys_from_sdl2_event(SDL_Keysym const&, bool key_is_pressed);
}