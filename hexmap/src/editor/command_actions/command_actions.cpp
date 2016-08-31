#include "command_actions.h"

namespace asdf {
namespace hexmap {
namespace editor
{
    void action_stack_t::push(std::unique_ptr<editor_action_t>&& action)
    {
        actions.push_back(std::move(action));
    }

    void action_stack_t::push_and_execute(std::unique_ptr<editor_action_t>&& action)
    {
        action->execute();
        push(std::move(action));
    }

    void action_stack_t::pop()
    {
        actions.pop_back();
    }

    void action_stack_t::pop_and_unexecute()
    {
        actions[actions.size() - 1]->unexecute();
        pop();
    }


    paint_tiles_action_t::paint_tiles_action_t(data::hex_grid_t& grid, tile_coord_list_t const& tiles, uint32_t new_id)
    : hex_grid(grid)
    , painted_tiles(tiles)
    , new_tile_id(new_id)
    {
    }

    void paint_tiles_action_t::execute()
    {
        old_ids.clear();
        
        for(size_t i = 0; i < painted_tiles.size(); ++i)
        {
            auto& tile = hex_grid.cell_at(painted_tiles[i]);
            old_ids.push_back(tile.tile_id);
            tile.tile_id = new_tile_id;
        }
    }

    void paint_tiles_action_t::unexecute()
    {
        for(size_t i = 0; i < painted_tiles.size(); ++i)
        {
            auto& tile = hex_grid.cell_at(painted_tiles[i]);
            tile.tile_id = old_ids[i];
        }
    }

    void resize_grid_action_t::execute()
    {
        
    }
    
    void resize_grid_action_t::unexecute()
    {

    }
}
}
}