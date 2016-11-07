#include "stdafx.h"
#include "mouse.h"

#include <array>

#include <SDL2/SDL_keyboard.h>

namespace asdf
{
    bool mouse_input_t::is_dragging(mouse_button_e btn) const
    {
        return mouse_button_states == btn && drag_delta().length() > drag_threshold_px;
    }

    glm::ivec2 mouse_input_t::drag_delta() const
    {
        return mouse_position - mouse_down_pos;
    }   


    void sdl2_mouse_input_t::on_event(SDL_Event* event)
    {
        ASSERT(thing, "expected thing (I should rename thing)");

        switch(event->type)
        {
            case SDL_MOUSEBUTTONDOWN:
            {
                mouse_button_states |= mouse_button_bit(mouse_left)   * (event->button.button == SDL_BUTTON_LEFT);
                mouse_button_states |= mouse_button_bit(mouse_right)  * (event->button.button == SDL_BUTTON_RIGHT);
                mouse_button_states |= mouse_button_bit(mouse_middle) * (event->button.button == SDL_BUTTON_MIDDLE);
                mouse_button_states |= mouse_button_bit(mouse_4)      * (event->button.button == SDL_BUTTON_X1);
                mouse_button_states |= mouse_button_bit(mouse_5)      * (event->button.button == SDL_BUTTON_X2);

                mouse_position.x = event->button.x;
                mouse_position.y = event->button.y;

                mouse_down_pos = mouse_position;

                auto asdf_event = mouse_event_from_sdl(*this, event->button);
                thing->on_mouse_down(asdf_event);
                break;
            }

            case SDL_MOUSEBUTTONUP:
            {
                mouse_button_states &= mouse_button_bit(mouse_left)   * (event->button.button != SDL_BUTTON_LEFT);
                mouse_button_states &= mouse_button_bit(mouse_right)  * (event->button.button != SDL_BUTTON_RIGHT);
                mouse_button_states &= mouse_button_bit(mouse_middle) * (event->button.button != SDL_BUTTON_MIDDLE);
                mouse_button_states &= mouse_button_bit(mouse_4)      * (event->button.button != SDL_BUTTON_X1);
                mouse_button_states &= mouse_button_bit(mouse_5)      * (event->button.button != SDL_BUTTON_X2);

                mouse_position.x = event->button.x;
                mouse_position.y = event->button.y;

                auto asdf_event = mouse_event_from_sdl(*this, event->button);
                thing->on_mouse_up(asdf_event);
                break;
            }

            case SDL_MOUSEMOTION:
            {
                mouse_position.x = event->motion.x;
                mouse_position.y = event->motion.y;

                auto asdf_event = mouse_event_from_sdl(*this, event->motion);
                thing->on_mouse_move(asdf_event);
                break;
            }

            case SDL_MOUSEWHEEL:
            {
                auto asdf_event = mouse_event_from_sdl(*this, event->wheel);
                thing->on_mouse_wheel(asdf_event);
                break;
            }
        }
    }


    constexpr std::array<mouse_button_e, num_mouse_buttons> sdl_mouse_button_conversion =
        { mouse_no_button, mouse_left, mouse_middle, mouse_right, mouse_4, mouse_5 };

    mouse_button_event_t mouse_event_from_sdl(mouse_input_t& mouse, SDL_MouseButtonEvent const& event)
    {
        return mouse_button_event_t {
              mouse
            , sdl_mouse_button_conversion[event.button]
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