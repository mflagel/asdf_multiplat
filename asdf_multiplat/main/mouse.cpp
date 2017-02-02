#include "stdafx.h"
#include "mouse.h"

#include <array>

#include <SDL2/SDL_keyboard.h>

namespace asdf
{
    bool mouse_input_t::is_dragging(mouse_button_e btn) const
    {
        return mouse_button_states == mouse_button_bit(btn) && drag_delta().x > drag_threshold_px && drag_delta().y > drag_threshold_px;
    }

    glm::ivec2 mouse_input_t::drag_delta() const
    {
        return mouse_position - mouse_down_pos;
    }

    void mouse_input_t::mouse_down(mouse_button_event_t& event, glm::ivec2 mouse_pos)
    {
        mouse_button_states |= mouse_button_bit(mouse_left)   * (event.button == mouse_left);
        mouse_button_states |= mouse_button_bit(mouse_right)  * (event.button == mouse_right);
        mouse_button_states |= mouse_button_bit(mouse_middle) * (event.button == mouse_middle);
        mouse_button_states |= mouse_button_bit(mouse_4)      * (event.button == mouse_4);
        mouse_button_states |= mouse_button_bit(mouse_5)      * (event.button == mouse_5);

        mouse_position = mouse_pos;
        mouse_down_pos = mouse_pos;

        thing->on_mouse_down(event);
    }

    void mouse_input_t::mouse_up(mouse_button_event_t& event, glm::ivec2 mouse_pos)
    {
        mouse_button_states &= mouse_button_bit(mouse_left)   * (event.button != mouse_left);
        mouse_button_states &= mouse_button_bit(mouse_right)  * (event.button != mouse_right);
        mouse_button_states &= mouse_button_bit(mouse_middle) * (event.button != mouse_middle);
        mouse_button_states &= mouse_button_bit(mouse_4)      * (event.button != mouse_4);
        mouse_button_states &= mouse_button_bit(mouse_5)      * (event.button != mouse_5);

        mouse_position = mouse_pos;

        thing->on_mouse_up(event);
    }

    void mouse_input_t::mouse_move(mouse_motion_event_t& event, glm::ivec2 mouse_pos)
    {
        mouse_position = mouse_pos;

        thing->on_mouse_move(event);
    }

    void mouse_input_t::mouse_wheel(mouse_wheel_event_t& event)
    {
        thing->on_mouse_wheel(event);
    }
}