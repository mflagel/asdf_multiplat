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

    mouse_button_event_t mouse_event_from_sdl(mouse_input_t&, SDL_MouseButtonEvent const&);
    mouse_motion_event_t mouse_event_from_sdl(mouse_input_t&, SDL_MouseMotionEvent const&);
    mouse_wheel_event_t  mouse_event_from_sdl(mouse_input_t&, SDL_MouseWheelEvent const&);

    uint32_t modifier_keys_from_sdl2_event(SDL_Keysym const&, bool key_is_pressed);
}