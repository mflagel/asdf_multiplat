#pragma once

#include <memory>

#include "main/hexmap.h"
#include "editor/main/input.h"
#include "editor/command_actions/command_actions.h"


namespace asdf {
namespace hexmap {
namespace editor
{

    enum hex_region_e
    {
          hex_no_region
        , hex_top_left
        , hex_top_right
        , hex_left
        , hex_right
        , hex_bottom_left
        , hex_bottom_right
        , hex_center
        , num_hex_regions
    };

    struct editor_t : hexmap_t
    {
        enum tool_type_e
        {
              select = 0
            , terrain_paint
            , place_objects
            , place_splines
            , num_tool_types
        };

        //terrain
        uint64_t current_tile_id = 0;
        tile_coord_dict_t painted_terrain_coords;

        //objects
        uint64_t current_object_id = 0;
        hex_region_e current_snap_point = hex_no_region;
        size_t selected_object_index = -1;


        tool_type_e current_tool = terrain_paint;

        std::unique_ptr<input_handler_t> input;
        action_stack_t action_stack;

        editor_t();
        void init() override;

        void render() override;

        void save_action();
        void load_action();

        bool undo();
        bool redo();

        void on_event(SDL_Event*) override;

        void set_tool(tool_type_e const& new_tool);
        
        void select_object(size_t object_index);
        size_t select_object_at(glm::vec2 position);
        bool is_object_selected() const { return selected_object_index != size_t(-1); }

        void paint_terrain_start();
        bool paint_terrain_at_coord(glm::ivec2 coord);
        void paint_terrain_end();

        void place_object(glm::vec2 position);
        void delete_object(size_t object_index);
    };


    constexpr std::array<const char*, editor_t::num_tool_types> tool_type_strings =
    {
          "select"
        , "terrain_paint"
        , "place_object"
        , "place_spline"
    };

}
}
}