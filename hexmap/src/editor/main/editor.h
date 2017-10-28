#pragma once

#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "main/hexmap.h"
#include "data/hex_map.h"
#include "data/hex_util.h"
#include "data/terrain_brush.h"
#include "ui/minimap.h"
#include "ui/terrain_brush_renderer.h"

#include "editor/main/input.h"
#include "editor/main/editor_workspace.h"
#include "editor/command_actions/command_actions.h"

namespace asdf {
namespace hexmap {
namespace editor
{
    using spline_index_t = data::spline_index_t;
    using spline_node_index_t = data::spline_node_index_t;

    
    struct base_selection_t
    {
        glm::vec2 upper_bound;
        glm::vec2 lower_bound;

        editor_t& editor;
    };

    struct object_selection_t : base_selection_t
    {
        std::unordered_set<size_t> object_indices;
        std::unordered_set<size_t> spline_indices;

        object_selection_t(editor_t&);

        bool operator ==(object_selection_t const& rhs) const;
        bool operator !=(object_selection_t const& rhs) const;

        bool add_object_index(size_t);
        bool remove_object_index(size_t);
        void clear_selection();
        bool is_empty() const { return object_indices.empty() && spline_indices.empty(); }

        template<typename L>
        void add_object_indices(L indices)
        {
            object_indices.insert(indices.begin(), indices.end());
            recalc_bounds();
        }

        void recalc_bounds();
    };

    struct spline_node_selection_t : base_selection_t
    {
        std::unordered_map<size_t, std::unordered_set<size_t>> node_indices; //map of spline inds, set of spline node inds

        spline_node_selection_t(editor_t&);

        bool operator ==(spline_node_selection_t const& rhs) const { return node_indices == rhs.node_indices; }
        bool operator !=(spline_node_selection_t const& rhs) const { return !(*this == rhs); }

        bool add_node_index(spline_index_t, spline_node_index_t);
        bool remove_node_index(spline_index_t, spline_node_index_t);
        bool add_all_nodes_from_spline(spline_index_t);
        bool remove_all_nodes_from_spline(spline_index_t);

        void clear_selection();

        bool is_empty() const { return node_indices.empty(); }

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

        enum drag_type_e
        {
              drag_type_none = 0
            , drag_selection_box
            , drag_selected_items
            , drag_type_count
        };


        //terrain
        uint64_t current_tile_id = 0;
        tile_coord_dict_t painted_terrain_coords;

        data::terrain_brush_t terrain_brush;
        std::vector<data::terrain_brush_t> saved_terrain_brushes;
        std::unique_ptr<ui::terrain_brush_renderer_t> terrain_brush_renderer;
        glm::vec2 brush_pos;

        //objects
        uint64_t current_object_id = 0;
        object_selection_t object_selection;
        data::map_object_t wip_object;

        //spline
        data::line_node_t new_node_style;
        data::spline_t::interpolation_e spline_interpolation_type = data::spline_t::linear;
        data::spline_t* wip_spline = nullptr; //the last node will follow the mouse
        data::line_node_t* wip_spline_node = nullptr;
        data::spline_selection_t spline_selection;

        ///
        hex_snap_flags_t snap_mode = hex_snap_center;
        float snap_threshold = 0.0f;
        drag_type_e drag_type = drag_type_none;
        glm::vec2 drag_start;
        glm::vec2 current_drag_position;
        glm::vec2 movement_drag_start_lower_bound; //lower bound of selection box
        glm::vec2 prev_lower_bound;


        ///
        editor_workspace_t workspace;
        std::string map_filepath;
        bool map_is_dirty = false;
        std::function<void()> map_changed_callback;

        tool_type_e current_tool = terrain_paint;

        std::unique_ptr<input_handler_t> input;

    private:
        action_stack_t action_stack; //private to ensure signal_data_changed() gets called

    public:
        editor_t();
        void init() override;
        void resize(uint32_t w, uint32_t h) override;

        void render() override;
        void render_selection();
        void render_current_brush();

        void on_event(SDL_Event*) override;

        void save_workspace(std::string const& filepath);
        void load_workspace(std::string const& filepath);

        void new_map_action(std::string const& map_name, glm::uvec2 const& size, data::hex_grid_cell_t const& default_cell_style = data::hex_grid_cell_t{});
        void save_action();
        void save_action(std::string const& filepath);
        void load_action(std::string const& filepath);

        bool undo();
        bool redo();

        void set_tool(tool_type_e const& new_tool);
        void set_current_tile_id(uint64_t new_id);
        void set_current_object_id(uint64_t new_id);
        void set_spline_node_style(data::line_node_t const& style);
        void set_current_spline_interpolation(data::spline_t::interpolation_e new_interp_type);
        void set_snap_settings(hex_snap_flags_t flags);
        void set_snap_settings(hex_snap_flags_t flags, float snap_threshold);

        std::tuple<glm::vec2,glm::vec2> selection_box_bounds() const;
        bool select_object(size_t object_index);
        bool deselect_object(size_t object_index);
        void deselect_all();

        bool select_object_at(glm::vec2 position);
        bool is_object_selected(size_t obj_index) const;

        void signal_data_changed();
        void push_action(std::unique_ptr<editor_action_t>&&);
        void push_and_execute_action(std::unique_ptr<editor_action_t>&&);

        void set_custom_terrain_brush(data::terrain_brush_t const& new_brush);
        inline data::terrain_brush_t const& current_terrain_brush() const {
            return terrain_brush;
        }
        void save_current_terrain_brush();

        void start_drag_selection(glm::vec2 const& world_pos);
        void update_drag_selection(glm::vec2 const& world_pos);
        void end_drag_selection(glm::vec2 const& world_pos);

        void start_drag_movement(glm::vec2 const& world_pos);
        void update_drag_movement(glm::vec2 const& world_pos);
        void end_drag_movement(glm::vec2 const& world_pos);

        void transform_selection(glm::vec2 const& new_lower_bounds/*, glm::vec2 const& new_upper_bounds*/);
        void delete_selected_objects();

        void paint_terrain_start();
        bool paint_terrain_at_coord(glm::ivec2 coord);
        bool paint_terrain_along_line(glm::vec2 const& p1_world, glm::vec2 const& p2_world, float sample_tick = 0.25f);
        void paint_terrain_end();

        void place_object(glm::vec2 position);
        void delete_object(size_t object_index);
        // data::map_object_t& wip_object();
        // data::map_object_t const& wip_object() const;

        void spline_click(glm::vec2 position);
        void start_spline(data::line_node_t start);
        void update_WIP_node(glm::vec2 const& position);
        void update_WIP_control_nodes(glm::vec2 const& position);
        void add_node_to_wip_spline(data::line_node_t node);
        void finish_spline(bool spline_loops = false);
        void cancel_spline();
        bool is_placing_spline() const { return wip_spline != nullptr; }

        void cancel_action();

    private:
        data::map_object_t __placeable_object(glm::vec2 position);
        void enable_wip_object();
        void disable_wip_object();
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
                add control nodes for 0th node

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
