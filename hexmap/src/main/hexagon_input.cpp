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

        /*       __
                /   
            __ /    
               \    
      hex  0,0  \ __
    world  0,0  /
            __ /
               \
                \ __
              
        world (0,0) is the center of the (0,0)th hexagon. add hexagon halfsize to the world pos to make world 0,0 the bottom left of the hexagon
        hex coords range from (-0.5,-0.5) to (0.5,0.5)

        everything within the angled slashes could be in the left or right hex depending
        on the y coord. the edges are at 60 degree angles (60 and 180-60)

        relevant edge can be determined by y position
                
        */
    glm::vec2 hex_map_input_t::hex_coords_from_mouse(glm::ivec2 mouse_pos)
    {
        auto mouse_world = world_coords();
        mouse_world += vec2(hex_width_d2, hex_height_d2); //

        //convert mouse world coords to a hexagon coord
        float sub_column = mouse_world.x / hex_width_d4;
        float row = mouse_world.y / hex_height;
        float sub_row = mouse_world.y / hex_height_d2;

        LOG("subcol: %f   subrow: %f", sub_column, sub_row);

        int column = glm::floor(sub_column / 3.0f);

        //if column is within hex_width_d4 of the column center (ie: fraction is +- 0.25) then it's only one column
        if((int)glm::floor(sub_column) % 3 == 0) //horizontal overlap every 3 sub-columns (with a width of one sub-column)
        {
            //todo: handle column overlap

            bool even = (int)abs(floor(sub_row)) % 2 == 0;
            //even rows slant right  '/'
            //odd rows slant left    '\'

            if(even)
            {LOG("overlap even  /");}
            else
            {LOG("overlap odd   \\");}
            
            //column += (intersecting right hex)
        }

        return vec2(sub_column/3.0f, row);
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
            auto hx = hex_coords_from_mouse(ms);
            LOG("hex coords: {%0.2f, %0.2f}", hx.x, hx.y);
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