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
    , hex_grid(_hex_map->map_data.hex_grid)
    , camera(_camera)
    , mouse_input(make_unique<sdl2_mouse_input_t>())
    {
        
    }


    bool hex_map_input_t::on_event(SDL_Event* event)
    {
        mouse_input->on_event(event);

        return false;
    }


    glm::vec2 hex_map_input_t::world_coords(glm::ivec2 const& screen_coord) const
    {
        return vec2(hex_map->camera.screen_to_world_coord(vec2(screen_coord)));
    }

        /*        __
        \  0,1  /   
         \ ___ /  1,1
         /     \    
      hex  0,0  \ __
    world  0,0  /
         \ ___ /  1,0
               \
                \ __
              
        world (0,0) is the center of the (0,0)th hexagon. add hexagon halfsize to the world pos to make world 0,0 the bottom left of the hexagon
        hex coords range from (-0.5,-0.5) to (0.5,0.5)

        everything within the angled slashes could be in the left or right hex depending
        on the y coord. the edges are at 60 degree angles (60 and 180-60)

        relevant edge can be determined by y position
                
        */
    glm::ivec2 hex_map_input_t::hex_coords_from_mouse() const
    {
        return world_to_hex_coord(world_coords(mouse_input->mouse_position));
    }
}
}
}