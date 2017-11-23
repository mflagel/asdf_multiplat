#include "stdafx.h"
#include "mouse.h"

#include <array>

#include <SDL2/SDL_keyboard.h>

namespace asdf
{
    glm::ivec2 mouse_input_t::move_delta() const
    {
        return mouse_position - mouse_prev_position;
    }

    bool mouse_input_t::is_dragging(mouse_button_e btn) const
    {
        return drag_states[btn].is_dragging && (drag_dist(btn) > drag_threshold_px);
    }

    bool mouse_input_t::mouse_input_t::is_dragging(mouse_button_flags_t mouse_btns) const
    {
        bool dragging = true;

        for(size_t i = 0; i < num_mouse_buttons; ++i)
        {
            if( (mouse_btns & mouse_button_bit(mouse_button_e(i))) > 0 )
            {
                dragging &= is_dragging(mouse_button_e(i));
            }
        }

        return dragging;
    }

    bool mouse_input_t::is_dragging_any_of(mouse_button_flags_t mouse_btns) const
    {
        bool dragging = false;

        for(size_t i = 0; i < num_mouse_buttons; ++i)
        {
            dragging |= is_dragging(mouse_button_e(i));
        }

        return dragging;
    }

    glm::ivec2 mouse_input_t::drag_delta(mouse_button_e btn) const
    {
        return mouse_position - drag_states[btn].mouse_down_pos;
    }

    int mouse_input_t::drag_dist(mouse_button_e btn) const
    {
        return glm::length(drag_delta(btn));
    }


    void mouse_input_t::mouse_down(mouse_button_event_t& event, glm::ivec2 mouse_pos)
    {
        mouse_prev_position = mouse_position;
        mouse_position = mouse_pos;

        mouse_button_states |= mouse_button_bit(mouse_left)   * (event.button == mouse_left);
        mouse_button_states |= mouse_button_bit(mouse_right)  * (event.button == mouse_right);
        mouse_button_states |= mouse_button_bit(mouse_middle) * (event.button == mouse_middle);
        mouse_button_states |= mouse_button_bit(mouse_4)      * (event.button == mouse_4);
        mouse_button_states |= mouse_button_bit(mouse_5)      * (event.button == mouse_5);

        drag_states[event.button].mouse_down_pos = mouse_pos;
        drag_states[event.button].is_dragging = true;

        event.handled |= receiver->on_mouse_down(event);
    }

    void mouse_input_t::mouse_up(mouse_button_event_t& event, glm::ivec2 mouse_pos)
    {
        mouse_button_states &= mouse_button_bit(mouse_left)   * (event.button != mouse_left);
        mouse_button_states &= mouse_button_bit(mouse_right)  * (event.button != mouse_right);
        mouse_button_states &= mouse_button_bit(mouse_middle) * (event.button != mouse_middle);
        mouse_button_states &= mouse_button_bit(mouse_4)      * (event.button != mouse_4);
        mouse_button_states &= mouse_button_bit(mouse_5)      * (event.button != mouse_5);

        mouse_prev_position = mouse_position;
        mouse_position = mouse_pos;

        event.handled |= receiver->on_mouse_up(event);

        //set after event, so that the event knows if the user is releasing a drag
        drag_states[event.button].is_dragging = false;
    }

    void mouse_input_t::mouse_drag(mouse_motion_event_t& event, glm::ivec2 mouse_pos)
    {
        mouse_prev_position = mouse_position;
        mouse_position = mouse_pos;

        event.handled |= receiver->on_mouse_drag(event);
    }

    void mouse_input_t::mouse_move(mouse_motion_event_t& event, glm::ivec2 mouse_pos)
    {
        mouse_prev_position = mouse_position;
        mouse_position = mouse_pos;

        event.handled |= receiver->on_mouse_move(event);
    }

    void mouse_input_t::mouse_wheel(mouse_wheel_event_t& event)
    {
        event.handled |= receiver->on_mouse_wheel(event);
    }
}
