#include "stdafx.h"  //somehow if this isnt included, MSVC gives weird errors in hexmap.h
#include "hexagon_input.h"

#include <SDL2/SDL_mouse.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

namespace asdf
{
namespace hexmap
{
namespace input
{
    
    hex_map_input_t::hex_map_input_t(ui::hex_map_t* _hex_map, camera_t& _camera)
    : hex_map(_hex_map)
    , hex_grid(_hex_map->hex_grid)
    , camera(_camera)
    , mouse_input(make_unique<sdl2_mouse_input_t>())
    {
        
    }

    glm::vec2 hex_map_input_t::world_coords() const
    {
        return vec2(hex_map->camera.screen_to_world_coord(vec2(mouse_input->mouse_position)));
    }

    glm::ivec2 hex_map_input_t::hex_coords_from_mouse(glm::ivec2 mouse_pos)
    {
        auto mouse_world = world_coords();

        //convert mouse world coords to a hexagon coord

        //figure out what column(s) the world coord could be intersecting
            //new column every (hex_width_d4 * 3) units
        auto column = mouse_world.x / (hex_width_d4 * 3);

        //figure out what cell(s) the world coord could be intersecting

        //test each cell for intersection

        //auto hex_coords = hex_coords_from_mouse(mouse_input->mouse_position);

        return ivec2(0,0);
    }


    bool hex_map_input_t::on_event(SDL_Event* event)
    {
        mouse_input->on_event(event);

        //TEST   (one would argue I should try unit tests
        if(event->type == SDL_MOUSEBUTTONDOWN)
        {
            auto const& ms = mouse_input->mouse_position;
            auto mw = world_coords();
            LOG("mouse_screen: {%i, %i}   mouse_world: {%0.2f, %0.2f}", ms.x, ms.y, mw.x, mw.y);
        }
        //---

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