#include "stdafx.h"  //somehow if this isnt included, MSVC gives weird errors in hexmap.h
#include "hexagon_input.h"

#include <SDL2/SDL_mouse.h>

using namespace std;
using namespace glm;

namespace asdf
{
namespace hexmap
{
namespace input
{
    
    hex_map_input_t::hex_map_input_t(data::hex_grid_t& _hex_grid, camera_t& _camera)
    : hex_grid(_hex_grid)
    , camera(_camera)
    , mouse_input(sdl2_mouse_input_t())
    {
        
    }

    glm::ivec2 hex_map_input_t::hex_coords_from_mouse(glm::vec2 mouse_pos)
    {
        EXPLODE("todo");
        return ivec2(0,0);
    }


    bool hex_map_input_t::on_event(SDL_Event* event)
    {
        mouse_input.on_event(event);

        //auto hex_coords = 

        return false;
    }


/// ----
    constexpr uint8_t mouse_btn_bit(mouse_button_e btn)
    {
        return ((uint8_t)(btn) + 1);
    }

    void sdl2_mouse_input_t::on_event(SDL_Event* event)
    {
        switch(event->type)
        {
            case SDL_MOUSEBUTTONDOWN:
            {
                mouse_button_states |= mouse_btn_bit(mouse_right)  * (event->button.button == SDL_BUTTON_LEFT);
                mouse_button_states |= mouse_btn_bit(mouse_right)  * (event->button.button == SDL_BUTTON_RIGHT);
                mouse_button_states |= mouse_btn_bit(mouse_middle) * (event->button.button == SDL_BUTTON_MIDDLE);
                mouse_button_states |= mouse_btn_bit(mouse_4)      * (event->button.button == SDL_BUTTON_X1);
                mouse_button_states |= mouse_btn_bit(mouse_5)      * (event->button.button == SDL_BUTTON_X2);

                mouse_position.x = event->button.x;
                mouse_position.y = event->button.y;
                break;
            }

            case SDL_MOUSEBUTTONUP:
                mouse_button_states &= mouse_btn_bit(mouse_right)  * (event->button.button != SDL_BUTTON_LEFT);
                mouse_button_states &= mouse_btn_bit(mouse_right)  * (event->button.button != SDL_BUTTON_RIGHT);
                mouse_button_states &= mouse_btn_bit(mouse_middle) * (event->button.button != SDL_BUTTON_MIDDLE);
                mouse_button_states &= mouse_btn_bit(mouse_4)      * (event->button.button != SDL_BUTTON_X1);
                mouse_button_states &= mouse_btn_bit(mouse_5)      * (event->button.button != SDL_BUTTON_X2);

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
}