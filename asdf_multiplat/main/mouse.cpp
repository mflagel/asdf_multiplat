#include "mouse.h"

#include <SDL2/SDL_keyboard.h>

namespace asdf
{
namespace input
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
                break;
            }

            case SDL_MOUSEBUTTONUP:
                mouse_button_states &= mouse_button_bit(mouse_left)   * (event->button.button != SDL_BUTTON_LEFT);
                mouse_button_states &= mouse_button_bit(mouse_right)  * (event->button.button != SDL_BUTTON_RIGHT);
                mouse_button_states &= mouse_button_bit(mouse_middle) * (event->button.button != SDL_BUTTON_MIDDLE);
                mouse_button_states &= mouse_button_bit(mouse_4)      * (event->button.button != SDL_BUTTON_X1);
                mouse_button_states &= mouse_button_bit(mouse_5)      * (event->button.button != SDL_BUTTON_X2);

                mouse_position.x = event->button.x;
                mouse_position.y = event->button.y;
                break;

            case SDL_MOUSEMOTION:
                mouse_position.x = event->motion.x;
                mouse_position.y = event->motion.y;
                break;

            case SDL_MOUSEWHEEL:
                break;
        }
    }

}
}