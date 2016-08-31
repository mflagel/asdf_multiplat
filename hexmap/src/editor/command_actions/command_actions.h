#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "editor/main/editor.h"

namespace asdf {
namespace hexmap {
namespace editor
{
    //undoable action
    struct editor_action_t
    {
        editor_action_t() = default;
        virtual ~editor_action_t() = default;

        virtual void execute() = 0;
        virtual void unexecute() = 0;
    };

    using tile_coord_list_t = std::vector<glm::ivec2>;

    struct action_stack_t
    {
        editor_t& editor;
        std::vector<std::unique_ptr<editor_action_t>> actions;

        void push(std::unique_ptr<editor_action_t>&&);
        void push_and_execute(std::unique_ptr<editor_action_t>&&);

        void pop();
        void pop_and_unexecute();
    };


    /// Action Subclasses
    struct paint_tiles_action_t : editor_action_t
    {
        data::hex_grid_t& hex_grid;
        tile_coord_list_t const& painted_tiles;
        uint32_t new_tile_id = 0;
        std::vector<uint32_t> old_ids;

        paint_tiles_action_t(data::hex_grid_t&, tile_coord_list_t const& painted_tiles, uint32_t new_tile_id);

        void execute() override;
        void unexecute() override;
    };

    struct resize_grid_action_t : editor_action_t
    {
        data::hex_grid_t& hex_grid
        glm::ivec2 prev_size;
        glm::ivec2 new_size;

        //std::vector<__something__> chopped_data; 

        void execute() override;
        void unexecute() override;
    };

}
}
}