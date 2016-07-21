#include "stdafx.h"  //somehow if this isnt included, MSVC gives weird errors in hexmap.h
#include "hexagon_input.h"

#include <SDL2/SDL_mouse.h>
#include <glm/gtc/matrix_transform.hpp>

#include "asdf_multiplat/main/mouse.h"
#include "asdf_multiplat/main/keyboard.h"
#include "asdf_multiplat/utilities/camera.h"

#include "data/hex_grid.h"

using namespace std;
using namespace glm;

namespace asdf
{
    using namespace input;

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


    bool hex_map_input_t::on_event(SDL_Event* event)
    {
        mouse_input->on_event(event);

        //TODO: only check/do this during certain sdl events
        if(mouse_input->mouse_button_state(mouse_left))
        {
            auto mw = world_coords();
            auto hx = hex_coords_from_mouse(mouse_input->mouse_position);

            LOG("mw: %.2f, %.2f   hx: %i, %i", mw.x, mw.y, hx.x, hx.y);

            if(hex_map->hex_grid.is_in_bounds(hx))
            {
                auto& cell = hex_map->hex_grid.cell_at(hx);
                cell.tile_id = 5; //test value
            }
        }

        return false;
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
    glm::ivec2 hex_map_input_t::hex_coords_from_mouse(glm::ivec2 mouse_pos)
    {
        auto mouse_world = world_coords();
        mouse_world += vec2(hex_width_d2, hex_height_d2); //adjust so that mouse world 0,0 is the bottom left of hexagon 0,0

        //
        auto const& ms = mouse_pos;
        auto const& mw = mouse_world;
        LOG("mouse_screen: {%i, %i}   mouse_world: {%0.2f, %0.2f}", ms.x, ms.y, mw.x, mw.y);
        //

        //convert mouse world coords to a hexagon coord
        float sub_column = mouse_world.x / hex_width_d4;
        float sub_row = mouse_world.y / hex_height_d2;

        LOG("subcol: %f   subrow: %f", sub_column, sub_row);

        int column = glm::floor(sub_column / 3.0f);
        int row = glm::floor(mouse_world.y / hex_height);

        //if column is within hex_width_d4 of the column center (ie: fraction is +- 0.25) then it's only one column
        if((int)glm::floor(sub_column) % 3 == 0) //horizontal overlap every 3 sub-columns (with a width of one sub-column)
        {
            //todo: handle column overlap

            bool even = (int)abs(floor(sub_row)) % 2 == 0;
            //even rows slant right  '/'
            //odd rows slant left    '\'

            vec2 line;

            auto angle = (even * 1.0f + PI) / 3.0f;
            line.x = cos(angle);
            line.y = sin(angle);
            line *= hex_edge_length;

            vec2 p0(floor(sub_column) * hex_width_d4, floor(sub_row) * hex_height_d2); //bottom point of slant

            auto p1 = p0 + line;
            auto const& p2 = mouse_world;
            auto side = (p1.x - p0.x)*(p2.y - p0.y) - (p1.y - p0.y)*(p2.x - p0.x);  //FIXME
            //((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x))
            
            
            LOG("side: %f", side);
            column -= 1 * (side < 0);
        }

        //if odd column, adjust row down
        if(column % 2 == 1)
        {
            row = floor((mouse_world.y + hex_height_d2) / hex_height);
        }

        return ivec2(column, row);
    }
}
}
}