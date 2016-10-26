#include "command_actions.h"

#include "asdf_multiplat/main/asdf_defs.h"

namespace asdf {
namespace hexmap {
namespace editor
{
    action_stack_t::action_stack_t(editor_t& _editor)
    : editor(_editor)
    {}

    void action_stack_t::undo()
    {
        ASSERT(can_undo(), "");
        pop_and_unexecute();
    }

    void action_stack_t::redo()
    {
        ASSERT(can_redo(), "");
        undone_actions.back()->execute();
        actions.push_back( std::move(undone_actions.back()) );
        undone_actions.pop_back();
    }

    bool action_stack_t::can_undo() const
    {
        return actions.size() > 0;
    }

    bool action_stack_t::can_redo() const
    {
        return undone_actions.size() > 0;
    }

    void action_stack_t::push(std::unique_ptr<editor_action_t>&& action)
    {
        actions.push_back(std::move(action));
        undone_actions.clear();
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
        ASSERT(actions.size() > 0, "popping an empty stack");
        actions.back()->unexecute();
        undone_actions.push_back(std::move(actions.back()));
        pop();
    }


    paint_tiles_action_t::paint_tiles_action_t(data::hex_grid_t& grid, tile_coord_dict_t tiles, uint32_t new_id)
    : hex_grid(grid)
    , painted_tiles(tiles)
    , new_tile_id(new_id)
    {
    }

    void paint_tiles_action_t::execute()
    {
        for(auto& tile : painted_tiles)
        {
            auto& coord = tile.first;
            ASSERT(hex_grid.is_in_bounds(coord), "committing paint to tile out of bounds");
            hex_grid.cell_at(coord).tile_id = new_tile_id;
        }
    }

    void paint_tiles_action_t::unexecute()
    {
        for(auto& tile : painted_tiles)
        {
            auto& coord = tile.first;
            hex_grid.cell_at(coord).tile_id = tile.second;
        }
    }

    // void resize_grid_action_t::execute()
    // {
        
    // }
    
    // void resize_grid_action_t::unexecute()
    // {

    // }
}
}
}