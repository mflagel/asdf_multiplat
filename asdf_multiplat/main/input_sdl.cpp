#include "stdafx.h"
#include "input_sdl.h"

namespace asdf
{
    void process_sdl_mouse_events(mouse_input_t& mouse, SDL_Event* event)
    {
        ASSERT(mouse.thing, "expected thing (I should rename thing)");

        switch(event->type)
        {
            case SDL_MOUSEBUTTONDOWN:
            {
                mouse.mouse_button_states |= mouse_button_bit(mouse_left)   * (event->button.button == SDL_BUTTON_LEFT);
                mouse.mouse_button_states |= mouse_button_bit(mouse_right)  * (event->button.button == SDL_BUTTON_RIGHT);
                mouse.mouse_button_states |= mouse_button_bit(mouse_middle) * (event->button.button == SDL_BUTTON_MIDDLE);
                mouse.mouse_button_states |= mouse_button_bit(mouse_4)      * (event->button.button == SDL_BUTTON_X1);
                mouse.mouse_button_states |= mouse_button_bit(mouse_5)      * (event->button.button == SDL_BUTTON_X2);

                mouse.mouse_position.x = event->button.x;
                mouse.mouse_position.y = event->button.y;

                mouse.mouse_down_pos = mouse.mouse_position;

                auto asdf_event = mouse_event_from_sdl(mouse, event->button);
                mouse.thing->on_mouse_down(asdf_event);
                break;
            }

            case SDL_MOUSEBUTTONUP:
            {
                mouse.mouse_button_states &= mouse_button_bit(mouse_left)   * (event->button.button != SDL_BUTTON_LEFT);
                mouse.mouse_button_states &= mouse_button_bit(mouse_right)  * (event->button.button != SDL_BUTTON_RIGHT);
                mouse.mouse_button_states &= mouse_button_bit(mouse_middle) * (event->button.button != SDL_BUTTON_MIDDLE);
                mouse.mouse_button_states &= mouse_button_bit(mouse_4)      * (event->button.button != SDL_BUTTON_X1);
                mouse.mouse_button_states &= mouse_button_bit(mouse_5)      * (event->button.button != SDL_BUTTON_X2);

                mouse.mouse_position.x = event->button.x;
                mouse.mouse_position.y = event->button.y;

                auto asdf_event = mouse_event_from_sdl(mouse, event->button);
                mouse.thing->on_mouse_up(asdf_event);
                break;
            }

            case SDL_MOUSEMOTION:
            {
                mouse.mouse_position.x = event->motion.x;
                mouse.mouse_position.y = event->motion.y;

                auto asdf_event = mouse_event_from_sdl(mouse, event->motion);
                mouse.thing->on_mouse_move(asdf_event);
                break;
            }

            case SDL_MOUSEWHEEL:
            {
                auto asdf_event = mouse_event_from_sdl(mouse, event->wheel);
                mouse.thing->on_mouse_wheel(asdf_event);
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