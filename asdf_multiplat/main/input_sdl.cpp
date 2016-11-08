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
            , mouse_button_e(uint32_t(event.button))
            , event.clicks > 1
            };
    }

    mouse_motion_event_t mouse_event_from_sdl(mouse_input_t& mouse, SDL_MouseMotionEvent const& event)
    {
        return mouse_motion_event_t{ mouse };
    }

    mouse_wheel_event_t  mouse_event_from_sdl(mouse_input_t& mouse, SDL_MouseWheelEvent const& event)
    {
        return mouse_wheel_event_t{
              mouse
            , event.y  // the amount scrolled vertically, positive away from the user and negative toward the user
        };
    }
}