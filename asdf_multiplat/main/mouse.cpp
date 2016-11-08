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
}