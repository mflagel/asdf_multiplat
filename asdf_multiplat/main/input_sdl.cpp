#include "stdafx.h"
#include "input_sdl.h"

using namespace std;
using namespace glm;

namespace asdf
{
    void process_sdl_mouse_events(mouse_input_t& mouse, SDL_Event* event)
    {
        ASSERT(mouse.thing, "expected thing (I should rename thing)");

        switch(event->type)
        {
            case SDL_MOUSEBUTTONDOWN:
            {
                auto asdf_event = mouse_event_from_sdl(mouse, event->button);
                mouse.mouse_down(asdf_event, ivec2(event->button.x, event->button.y));
                break;
            }

            case SDL_MOUSEBUTTONUP:
            {
                auto asdf_event = mouse_event_from_sdl(mouse, event->button);
                mouse.mouse_up(asdf_event, ivec2(event->button.x, event->button.y));
                break;
            }

            case SDL_MOUSEMOTION:
            {
                auto asdf_event = mouse_event_from_sdl(mouse, event->motion);
                mouse.mouse_move(asdf_event, ivec2(event->button.x, event->button.y));
                break;
            }

            case SDL_MOUSEWHEEL:
            {
                auto asdf_event = mouse_event_from_sdl(mouse, event->wheel);
                mouse.mouse_wheel(asdf_event);
                break;
            }
        }
    }

    mouse_button_event_t mouse_event_from_sdl(mouse_input_t& mouse, SDL_MouseButtonEvent const& event)
    {
        return mouse_button_event_t {
              mouse
            , mouse_button_from_sdl_btn(event.button)
            , event.clicks > 1
            };
    }

    mouse_motion_event_t mouse_event_from_sdl(mouse_input_t& mouse, SDL_MouseMotionEvent const& event)
    {
        ASDF_UNUSED(event);

        return mouse_motion_event_t{ mouse };
    }

    mouse_wheel_event_t  mouse_event_from_sdl(mouse_input_t& mouse, SDL_MouseWheelEvent const& event)
    {
        return mouse_wheel_event_t{
              mouse
            , event.y  // the amount scrolled vertically, positive away from the user and negative toward the user
        };
    }


    /// Apparently in SDL2 if the key in the keysm is a modifier key, it doesnt
    /// get OR'd into the keysm.mod value on key down.
    uint32_t modifier_keys_from_sdl2_event(SDL_Keysym const& keysm, bool key_is_pressed)
    {
        auto modifier_keys = keysm.mod;
        modifier_keys &= KMOD_SHIFT | KMOD_CTRL | KMOD_ALT | KMOD_GUI; //we only care about these flags

        if(key_is_pressed)
        {
            modifier_keys |= KMOD_LSHIFT * (keysm.sym == SDLK_LSHIFT);
            modifier_keys |= KMOD_RSHIFT * (keysm.sym == SDLK_RSHIFT);
            modifier_keys |= KMOD_LCTRL  * (keysm.sym == SDLK_LCTRL);
            modifier_keys |= KMOD_RCTRL  * (keysm.sym == SDLK_RCTRL);
            modifier_keys |= KMOD_LALT   * (keysm.sym == SDLK_LALT);
            modifier_keys |= KMOD_RALT   * (keysm.sym == SDLK_RALT);
            modifier_keys |= KMOD_LGUI   * (keysm.sym == SDLK_LGUI);
            modifier_keys |= KMOD_RGUI   * (keysm.sym == SDLK_RGUI);
        }

        return modifier_keys;
    }
}