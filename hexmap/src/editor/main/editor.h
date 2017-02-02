#pragma once

#include <memory>
#include <unordered_set>

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

    struct object_selection_t
    {
        glm::vec2 upper_bound;
        glm::vec2 lower_bound;

        editor_t& editor;
        std::unordered_set<size_t> object_indices;

        object_selection_t(editor_t&);

        bool add_object_index(size_t);
        bool remove_object_index(size_t);
        void clear_selection();
        bool is_empty() const { return object_indices.empty(); }

        void recalc_bounds();
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
        object_selection_t object_selection;

        //spline
        data::line_node_t new_node_style;
        data::spline_t::interpolation_e spline_interpolation_type = data::spline_t::linear;
        data::spline_t* wip_spline = nullptr; //the last node will follow the mouse
        data::line_node_t* wip_spline_node = nullptr;
        data::spline_selection_t spline_selection;

        ///
        tool_type_e current_tool = terrain_paint;

        std::unique_ptr<input_handler_t> input;
        action_stack_t action_stack;

        editor_t();
        void init() override;

        void render() override;
        void render_selection();

        void on_event(SDL_Event*) override;

        void new_map_action(glm::uvec2 const& size);
        void save_action();
        void load_action();

        bool undo();
        bool redo();

        void set_tool(tool_type_e const& new_tool);
        void set_current_tile_id(uint64_t new_id);
        void set_current_object_id(uint64_t new_id);
        void set_current_spline_interpolation(data::spline_t::interpolation_e new_interp_type);

        bool select_object(size_t object_index);
        bool deselect_object(size_t object_index);
        void deselect_all();

        bool select_object_at(glm::vec2 position);
        bool is_object_selected(size_t obj_index) const;

        void paint_terrain_start();
        bool paint_terrain_at_coord(glm::ivec2 coord);
        void paint_terrain_end();

        void place_object(glm::vec2 position);
        void delete_object(size_t object_index);

        void spline_click(glm::vec2 position);
        void start_spline(data::line_node_t start);
        void update_WIP_node(glm::vec2 const& position);
        void update_WIP_control_nodes(glm::vec2 const& position);
        void add_node_to_wip_spline(data::line_node_t node);
        void finish_spline(bool spline_loops = false);
        void cancel_spline();
        bool is_placing_spline() const { return wip_spline != nullptr; }

        void cancel_action();
    };


    constexpr std::array<const char*, editor_t::num_tool_types> tool_type_strings =
    {
          "select"
        , "terrain_paint"
        , "place_object"
        , "place_spline"
    };




    /*
        Spline Creation / Modification Notes  (as of Feb 1st 2017)

        Spline Creation Flow:
        {
            editor::input_handler_t::on_mouse_down()
                case editor_t::place_splines:
                    editor.spline_click(mw);

            editor_t::spline_click()
                if( !is_placing_spline() )
                    start_spline(node);
                else
                    if(clicking close to first point)
                        finish_spline()
                    else
                        add_node_to_wip_spline(node);

            editor_t::start_spline()
                create empty spline
                add start node to spline
                add a copy of start to act as the WIP node, which will move under the mouse
                add a control node

            editor_t::add_node_to_wip_spline(new_node)
                set contents of wip_node to that of new_node
                add a new node to the list; it is the new wip_node
                add control point(s) for the new wip node
    
        }


        Spline Control Point
        {
            on spline start
                add control node (same position as 0th node)

            while dragging with mouse down
                move position of WIP_control_node (same position as WIP_node)

            on mouse up
                WIP control point is no longer WIP
        }

    */

}
}
}