#pragma once

#include <array>

#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <glm/glm.hpp>

namespace asdf
{
    constexpr int drag_threshold_px = 10;

    enum mouse_button_e : uint32_t
    {
          mouse_no_button = 0
        , mouse_left
        , mouse_right
        , mouse_middle
        , mouse_4
        , mouse_5
        , num_mouse_buttons
    };

    constexpr uint32_t mouse_button_bit(mouse_button_e btn)
    {
        return 1u << static_cast<uint8_t>(btn);
    }

    struct mouse_input_t;

    struct mouse_motion_event_t
    {
        mouse_input_t& mouse_state;
    };

    struct mouse_button_event_t
    {
        mouse_input_t& mouse_state;
        mouse_button_e button;
        bool double_clicked;
    };

    struct mouse_wheel_event_t
    {
        mouse_input_t& mouse_state;
        int delta;
    };


    struct mouse_event_receiver_t
    {
        virtual ~mouse_event_receiver_t() = default;

        virtual bool on_mouse_down(mouse_button_event_t&) = 0;
        virtual bool on_mouse_up(mouse_button_event_t&) = 0;
        virtual bool on_mouse_move(mouse_motion_event_t&) = 0;
        virtual bool on_mouse_wheel(mouse_wheel_event_t&) = 0;
    };


    struct mouse_input_t
    {
        uint32_t mouse_button_states = 0;
        glm::ivec2 mouse_prev_position;
        glm::ivec2 mouse_position;
        glm::ivec2 mouse_down_pos;

        mouse_event_receiver_t* receiver = nullptr;

        ~mouse_input_t() = default;

        void mouse_down(mouse_button_event_t&, glm::ivec2 mouse_pos);
        void mouse_up(mouse_button_event_t&, glm::ivec2 mouse_pos);
        void mouse_move(mouse_motion_event_t&, glm::ivec2 mouse_pos);
        void mouse_wheel(mouse_wheel_event_t&);

        bool mouse_button_state(mouse_button_e btn) const
        {
            return (mouse_button_states & mouse_button_bit(btn)) > 0;
        }
        
        glm::ivec2 move_delta() const;
        bool is_dragging(mouse_button_e btn = mouse_left) const;
        glm::ivec2 drag_delta() const;
    };
}