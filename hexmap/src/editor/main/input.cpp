#include "stdafx.h"
#include "input.h"

#include "asdf_multiplat/main/mouse.h"

namespace asdf
{
    using namespace input;

namespace hexmap 
{
namespace editor
{
    bool input_handler_t::on_event(SDL_Event* event)
    {
        input::hex_map_input_t::on_event(event);

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
                return true;
            }
        }

        return false;
    }

}
}
}