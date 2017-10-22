#include "editor.h"

#include <limits>

#include <glm/gtc/matrix_transform.hpp>

#include "asdf_multiplat/data/content_manager.h"

#include "data/spline.h"

using namespace std;
using namespace glm;

namespace stdfs = std::experimental::filesystem;

namespace asdf
{
    using namespace util;
    using namespace data;

namespace hexmap {

    using spline_t = data::spline_t;

namespace editor
{
    const/*expr*/ color_t selection_overlay_color = color_t(1.0, 1.0, 1.0, 0.5f);
    const/*expr*/ glm::vec3 default_camera_position = glm::vec3(0.0f, 0.0f, 0.0f);

    constexpr char terrain_types_json_filename[] = "terrain_types.json";

    const/*expr*/ data::line_node_t default_spline_style = {
          vec2{0.0f,0.0f}                   //pos
        , 1.0f                            //thickness
        , color_t{0.0f, 0.5f, 1.0f, 1.0f} //color
    };

    constexpr float snap_dist_threshold = 0.1f; //in units

    //TODO: move this into an asdf_multiplat header
    constexpr bool is_sdl_keyboard_event(SDL_Event* event)
    {
        return event->type == SDL_KEYDOWN
            || event->type == SDL_KEYUP
            || event->type == SDL_TEXTEDITING
            || event->type == SDL_TEXTINPUT
            // || event->type == SDL_KEYMAPCHANGED  /// Does not compile on travis (it might be using an old version of SDL? docs say this requires SDL 2.0.4)
            ;
    }
    //--


    editor_t::editor_t()
    : hexmap_t()
    , action_stack(*this)
    , object_selection(*this)
    , new_node_style{default_spline_style}
    {}

    void editor_t::init()
    {
        hexmap_t::init();

#ifdef DEBUG
        new_map_action("", uvec2(16,16));
#else
        new_map_action("", uvec2(1,1));
#endif

        input = make_unique<input_handler_t>(*this);
        app.mouse_state.receiver = input.get();

        {
            using namespace data;
            saved_terrain_brushes.push_back(terrain_brush_hexagon(0));   //default point brush
            saved_terrain_brushes.push_back(terrain_brush_rectangle(3,3));   //default small rect
            saved_terrain_brushes.push_back(terrain_brush_rectangle(5,5));   //default medium rect
            saved_terrain_brushes.push_back(terrain_brush_rectangle(10,10)); //default large rect
            // saved_terrain_brushes.push_back(terrain_brush_circle(1.0f));     //default small circle
            // saved_terrain_brushes.push_back(terrain_brush_circle(3.0f));     //default medium circle
            // saved_terrain_brushes.push_back(terrain_brush_circle(5.0f));     //default large circle
            saved_terrain_brushes.push_back(terrain_brush_hexagon(1));
            saved_terrain_brushes.push_back(terrain_brush_hexagon(2));
            saved_terrain_brushes.push_back(terrain_brush_hexagon(3));
            saved_terrain_brushes.push_back(terrain_brush_hexagon(5));

            terrain_brush = saved_terrain_brushes[0];

            terrain_brush_renderer = std::make_unique<ui::terrain_brush_renderer_t>();
            terrain_brush_renderer->init(Content.create_shader_highest_supported("passthrough"));
            terrain_brush_renderer->set_brush(&terrain_brush);
        }
    }

    void editor_t::resize(uint32_t w, uint32_t h)
    {
        hexmap_t::resize(w, h);
    }


    void editor_t::render()
    {
        hexmap_t::render();

        render_current_brush();

        if(wip_spline)
        {
            std::vector<size_t> inds;
            inds.emplace_back(map_data.splines.size() - 1);
            rendered_map.spline_renderer.render_some_spline_handles(inds);
        }

        render_selection();

        ASSERT(!CheckGLError(), "GL Error in editor_t::render()");
    }

    void editor_t::render_selection()
    {
        //draw transparent white boxes over selected objects
        for(auto const& sel_obj_ind : object_selection.object_indices)
        {
            auto const& sel_obj = map_data.objects[sel_obj_ind];

            auto& spritebatch = rendered_map.spritebatch;
            auto const& shader = rendered_map.shader;
            spritebatch.begin(shader->view_matrix, shader->projection_matrix);

            //scale the pixel texture to the size of the object
            auto const& pixel_texture = Content.textures["pixel"];
            auto obj_size_px = sel_obj.size_d2 * 2.0f * px_per_unit;
            vec2 scale = vec2(uvec2(obj_size_px) / pixel_texture->get_size()); //scale overlay texture to match object texture size
            auto sprite_scale = scale * sel_obj.scale * units_per_px;

            spritebatch.draw(pixel_texture, sel_obj.position, selection_overlay_color, sprite_scale, sel_obj.rotation);

            spritebatch.end();
        }


        /// Setup for drawing bounding boxes
        auto& shader = rendered_map.shader;
        GL_State->bind(shader);

        auto const& camera = rendered_map.camera;
        shader->view_matrix       = camera.view_matrix();
        shader->projection_matrix = camera.projection_ortho();


        auto draw_box = [&shader](glm::vec2 const& lb, glm::vec2 const& ub)
        {
            glm::vec2 bbox_size = ub - lb;
            glm::vec2 trans = lb + bbox_size/2.0f;

            shader->world_matrix = mat4{};
            shader->world_matrix = glm::translate(shader->world_matrix, vec3(trans, 0.0f));
            shader->world_matrix = glm::scale(shader->world_matrix, vec3(bbox_size, 0.0f));

            shader->update_wvp_uniform();

            app.renderer->box.render(GL_LINE_LOOP);
        };



        /// Bounding Boxes
        if(object_selection.object_indices.size() > 0)
        {
            /// Selection
            draw_box(object_selection.lower_bound, object_selection.upper_bound);
        }

        /// Drag Selection
        if(drag_type == drag_selection_box)
        {
            auto sel_bounds = selection_box_bounds();
            draw_box(get<0>(sel_bounds), get<1>(sel_bounds));
        }
    }

    void editor_t::render_current_brush()
    {
        switch(current_tool)
        {
            case terrain_paint:
            {
                ASSERT(terrain_brush_renderer, "cannot render terrain brush without a terrain_brush_renderer");
                ASSERT(terrain_brush_renderer->shader, "terrain_brush_renderer has no shader");

                //TODO: setup terrain brush shader WVP
                auto& shader = terrain_brush_renderer->shader;

                shader->world_matrix = glm::mat4();

                shader->world_matrix[3][0] = brush_pos.x;
                shader->world_matrix[3][1] = brush_pos.y + hex_height_d2;

                shader->view_matrix       = rendered_map.shader->view_matrix ;
                shader->projection_matrix = rendered_map.shader->projection_matrix;

                terrain_brush_renderer->render();

                break;
            }

            case place_objects:
            {


                break;
            }
        };
    }


    void editor_t::new_map_action(std::string const& map_name, glm::uvec2 const& size, data::hex_grid_cell_t const& default_cell_style)
    {
        map_data.reset(map_name, size, default_cell_style);
        
        /// (re)-add the WIP object at the start of the list
        data::map_object_t obj = __placeable_object(vec2(0.0f));
        map_data.objects.insert(map_data.objects.begin(), std::move(obj));

        if(current_tool == place_objects) {
            enable_wip_object();
        } else {
            disable_wip_object();
        }

        map_filepath = "";
        map_is_dirty = false;

        action_stack.clear();

        //reset camera
        rendered_map.camera_controller.position = default_camera_position;
        rendered_map.update(0.0f);
        rendered_map.camera.viewport = viewport_for_size_aspect(map_data.hex_grid.size_units(), rendered_map.camera.aspect_ratio);

        signal_data_changed();
    }

    void editor_t::save_action()
    {
        save_action(map_filepath);
    }

    void editor_t::save_action(std::string const& filepath)
    {
        map_filepath = filepath;
        map_data.save_to_file(filepath);
        map_is_dirty = false;
        LOG("map saved to %s", filepath.c_str());

        workspace.update_recently_opened_with(filepath);
    }

    void editor_t::load_action(std::string const& filepath)
    {
        map_data.load_from_file(filepath);
        
        map_filepath = filepath;
        map_is_dirty = false;
        action_stack.clear();
        LOG("map loaded from %s", filepath.c_str());

        workspace.update_recently_opened_with(filepath);

        rendered_map.camera_controller.position = default_camera_position;
        rendered_map.update(0.0f);
        rendered_map.camera.viewport = viewport_for_size_aspect(map_data.hex_grid.size_units(), rendered_map.camera.aspect_ratio);

        signal_data_changed();
    }

    bool editor_t::undo()
    {
        if(action_stack.can_undo())
        {
            action_stack.undo();
            signal_data_changed();
            return true;
        }

        return false;
    }

    bool editor_t::redo()
    {
        if(action_stack.can_redo())
        {
            action_stack.redo();
            signal_data_changed();
            return true;
        }

        return false;
    }

    void editor_t::on_event(SDL_Event* event)
    {
        // I eventually want to remove this in place of my own keyboard
        // abstraction so I can get rid of handling sdl events here
        if(is_sdl_keyboard_event(event))
        {
            if(event->key.type == SDL_KEYDOWN && event->key.repeat == 0)
            {
                input->on_key_down(event->key.keysym);
            }
            else if(event->key.type == SDL_KEYUP && event->key.repeat == 0)
            {
                input->on_key_up(event->key.keysym);
            }
        }

        rendered_map.on_event(event); //for camera controller
    }


    void editor_t::save_workspace(std::string const& filepath)
    {
        json_to_file(workspace.to_JSON(), filepath);
    }

    void editor_t::load_workspace(std::string const& filepath)
    {
        ASSERT(stdfs::exists(stdfs::path(filepath)), "workspace filepath does not exist");
        
        cJSON* root = json_from_file(filepath);
        workspace.from_JSON(root);
        cJSON_Delete(root);
    }


    void editor_t::set_tool(editor_t::tool_type_e const& new_tool)
    {
        switch(current_tool)
        {
            case select:
            {
                object_selection.clear_selection();
                break;
            }
            case terrain_paint:
            {
                paint_terrain_end();
                break;
            }
            case place_objects:
            {
                disable_wip_object();
                break;
            }
            case place_splines:
            {
                break;
            }

            case num_tool_types: break;
        };

        current_tool = new_tool;

        switch(new_tool)
        {
            case place_objects:
                enable_wip_object();
                break;
        };


        LOG("current tool: %s", tool_type_strings[current_tool]);
    }

    void editor_t::set_current_tile_id(uint64_t new_id)
    {
        if(current_tile_id != new_id)
        {
        }

        current_tile_id = new_id;
        LOG("current tile_id: %ld", current_tile_id);
    }

    void editor_t::set_current_object_id(uint64_t new_id)
    {
        if(current_object_id != new_id)
        {
        }

        current_object_id = new_id;

        if(current_tool == place_objects)
            wip_object().id = new_id;

        LOG("current object_id: %ld", current_object_id);
    }

    void editor_t::set_spline_node_style(data::line_node_t const& style)
    {
        new_node_style = style;

        if(wip_spline_node)
        {
            *wip_spline_node = style;
        }
    }

    void editor_t::set_current_spline_interpolation(data::spline_t::interpolation_e new_interp_type)
    {
        if((int)new_interp_type >= (int)data::spline_t::num_interp_types)
            return;

        if(spline_interpolation_type != new_interp_type)
        {
            if(is_placing_spline())
            {
                finish_spline();
            }
        }

        spline_interpolation_type = new_interp_type;
        LOG("current interp type: %s", data::spline_interpolation_names[new_interp_type]);
    }



    /// Selection
    std::tuple<glm::vec2,glm::vec2> editor_t::selection_box_bounds() const
    {
        auto sel_lb = glm::min(drag_start, current_drag_position);
        auto sel_ub = glm::max(drag_start, current_drag_position);

        return std::tuple<glm::vec2,glm::vec2>(sel_lb, sel_ub);
    }

    bool editor_t::select_object(size_t object_index)
    {
        ASSERT(object_index != nullindex, "");
        // LOG("selected object: %zu;  %zu objects selected", object_index, object_selection.object_indices.size()+1);
        return object_selection.add_object_index(object_index);
    }

    bool editor_t::deselect_object(size_t object_index)
    {
        ASSERT(object_index != nullindex, "");
        // LOG("deselected object: %zu;  %zu objects selected", object_index, object_selection.object_indices.size()-1);
        return object_selection.remove_object_index(object_index);
    }

    void editor_t::deselect_all()
    {
        object_selection.clear_selection(); ///TODO rename to be consistent?
        spline_selection.deselect_all();
    }

    bool editor_t::select_object_at(glm::vec2 position)
    {
        size_t ind = map_data.object_index_at(position);

        if(ind != nullindex)
        {
            select_object(ind);
            return true;
        }

        return false;
    }

    bool editor_t::is_object_selected(size_t obj_index) const
    {
        return object_selection.object_indices.count(obj_index);
    }


    void editor_t::signal_data_changed()
    {
        if(map_changed_callback)
            map_changed_callback();
    }

    void editor_t::push_action(std::unique_ptr<editor_action_t>&& action)
    {
        action_stack.push(move(action));
        signal_data_changed();
    }

    void editor_t::push_and_execute_action(std::unique_ptr<editor_action_t>&& action)
    {
        action_stack.push_and_execute(move(action));
        signal_data_changed();
    }


    /// Terrain Brushes
    void editor_t::set_custom_terrain_brush(data::terrain_brush_t const& new_brush)
    {
        terrain_brush = new_brush;        
        terrain_brush_renderer->set_brush(&terrain_brush);
    }

    void editor_t::save_current_terrain_brush()
    {
        if(saved_terrain_brushes.empty())
            saved_terrain_brushes.push_back(terrain_brush);
        else if(terrain_brush != saved_terrain_brushes.back())
            saved_terrain_brushes.push_back(terrain_brush);
    }


    /// Drag Selection
    void editor_t::start_drag_selection(glm::vec2 const& world_pos)
    {
        ASSERT(drag_type == drag_selection_box, "Incorrect drag type for dragging selection box");

        drag_start = world_pos;
        current_drag_position = world_pos;
    }

    void editor_t::update_drag_selection(glm::vec2 const& world_pos)
    {
        ASSERT(drag_type == drag_selection_box, "Incorrect drag type for dragging selection box");

        current_drag_position = world_pos;
    }

    void editor_t::end_drag_selection(glm::vec2 const& world_pos)
    {
        ASSERT(drag_type == drag_selection_box, "Incorrect drag type for dragging selection box");

        update_drag_selection(world_pos);

        auto bounds = selection_box_bounds();
        auto inds = map_data.object_indices_within(get<0>(bounds), get<1>(bounds));
        object_selection.add_object_indices(inds);
    }


    /// Drag Movement
    void editor_t:: start_drag_movement(glm::vec2 const& world_pos)
    {
        ASSERT(drag_type == drag_selected_items, "Incorrect drag type for dragging selected items");
        ASSERT(!object_selection.is_empty(), "Dragging an empty selection box?");

        movement_drag_start_lower_bound = object_selection.lower_bound;
        prev_lower_bound = movement_drag_start_lower_bound;
    }

    void editor_t:: update_drag_movement(glm::vec2 const& world_pos)
    {
        ASSERT(drag_type == drag_selected_items, "Incorrect drag type for dragging selected items");

        //auto dist = movement_drag_start_lower_bound
        auto drag_dist = current_drag_position - drag_start;
        auto new_lb = movement_drag_start_lower_bound + drag_dist;

        transform_selection(new_lb);
    }

    void editor_t:: end_drag_movement(glm::vec2 const& world_pos)
    {
        ASSERT(drag_type == drag_selected_items, "Incorrect drag type for dragging selected items");

        update_drag_movement(world_pos);

        //todo: push command to action stack
    }


    /// Object Manipulation
    void editor_t::transform_selection(glm::vec2 const& new_lower_bounds/*, glm::vec2 const& new_upper_bounds*/)
    {
        /// Translate using lower bounds
        auto translation = new_lower_bounds - object_selection.lower_bound;

        object_selection.lower_bound = new_lower_bounds;
        //object_selection.upper_bound = new_upper_bounds;
        object_selection.upper_bound += translation;

        for(auto const& obj_ind : object_selection.object_indices)
        {
            map_data.objects[obj_ind].position += translation;
        }

        ///TODO: support scaling objects (using diff between bound sizes
    }

    void editor_t::delete_selected_objects()
    {
        if(!object_selection.is_empty())
        {
            auto cmd = make_unique<delete_map_objects_action_t>(map_data, object_selection.object_indices);
            push_and_execute_action(std::move(cmd));
        }
    }



    /// Terrain
    void editor_t::paint_terrain_start()
    {
        painted_terrain_coords.clear();
    }

    /// If the coord is inside the map, track the old tile_id of that cell
    /// and then paint it with the editor's current_tile_id
    bool editor_t::paint_terrain_at_coord(glm::ivec2 coord)
    {
        //test
        auto overlap_coords = data::get_brush_grid_overlap(current_terrain_brush(), map_data.hex_grid, coord);

        for(auto const& coord: overlap_coords)
        {
            ASSERT(map_data.hex_grid.is_in_bounds(coord), "overlap coord OOB");

            auto& cell = map_data.hex_grid.cell_at(coord);
            painted_terrain_coords.insert({coord, cell.tile_id});
            cell.tile_id = current_tile_id;
        }

        return overlap_coords.size() > 0;
        //

        //if(map_data.hex_grid.is_in_bounds(coord))
        //{
        //    auto& cell = map_data.hex_grid.cell_at(coord);
        //    painted_terrain_coords.insert({coord, cell.tile_id});
        //    cell.tile_id = current_tile_id;
        //    return true;
        //}

        return false;
    }


    bool editor_t::paint_terrain_along_line(glm::vec2 const& p1_world, glm::vec2 const& p2_world, float sample_tick)
    {
        auto vec = p2_world - p1_world;

        if(vec.x == 0.0f && vec.y == 0.0f)
        {
            return false;
        }

        auto len = glm::length(vec);
        auto unit = glm::normalize(vec);

        float sample_len = 0.0f;
        bool stuff_painted = false;
        do
        {
            auto sample_pos = p1_world + unit * sample_len;
            auto hx = world_to_hex_coord(sample_pos);

            stuff_painted |= paint_terrain_at_coord(hx);

            sample_len += sample_tick;
        }
        while(sample_len + sample_tick < len);

        return stuff_painted;
    }

    void editor_t::paint_terrain_end()
    {
        if(!painted_terrain_coords.empty())
        {
            push_action(make_unique<paint_tiles_action_t>
                ( map_data.hex_grid
                , std::move(painted_terrain_coords)
                , current_tile_id
                )
            );

            painted_terrain_coords.clear(); //pretty sure its empty anyway due to std::move
        }
    }


    /// Map Objects
    void editor_t::place_object(glm::vec2 position)
    {
        auto obj = __placeable_object(position);
        push_and_execute_action(make_unique<add_map_object_action_t>(map_data, std::move(obj)));
    }

    void editor_t::delete_object(size_t object_index)
    {
        auto cmd = make_unique<delete_map_objects_action_t>(map_data, object_index);
        push_and_execute_action(std::move(cmd));
    }

    data::map_object_t& editor_t::wip_object()
    {
        ASSERT(map_data.objects.size() > 0, "The WIP object has been deleted (or was never added)");
        return map_data.objects[0];
    }

    data::map_object_t const& editor_t::wip_object() const
    {
        ASSERT(map_data.objects.size() > 0, "The WIP object has been deleted (or was never added)");
        return this->map_data.objects[0];
    }


    void editor_t::spline_click(glm::vec2 position)
    {
        auto node = new_node_style;
        node.position = position;
        

        if(!is_placing_spline())
        {
            start_spline(node);
        }
        else
        {
            auto dist = glm::length(position - wip_spline->nodes[0].position);
            if(dist <= snap_dist_threshold)
            {
                finish_spline(true);
            }
            else
            {
                add_node_to_wip_spline(node);
            }
        }
    }

    void editor_t::start_spline(data::line_node_t start)
    {
        LOG("starting spline at (%f,%f)", start.position.x, start.position.y);

        data::spline_t spline;
        spline.spline_type = spline_interpolation_type;
        spline.nodes.push_back(start);  //0th node is a copy of start
        spline.nodes.push_back(std::move(start)); //use start as the second node to move under the mouse (the WIP node)

        map_data.splines.push_back(std::move(spline));
        wip_spline = &(map_data.splines.back());
        wip_spline_node = &(wip_spline->nodes.back());

        if(wip_spline->spline_type == spline_t::bezier)
        {
            //these belong to the 0th node
            wip_spline->control_nodes.emplace_back(wip_spline_node->position);
            wip_spline->control_nodes.emplace_back(wip_spline_node->position);

            //these belong to the WIP node
            wip_spline->control_nodes.emplace_back(wip_spline_node->position);
            wip_spline->control_nodes.emplace_back(wip_spline_node->position);
        }
    }

    void editor_t::update_WIP_node(glm::vec2 const& position)
    {
        ASSERT(wip_spline, "");
        wip_spline->nodes.back().position = position;

        ptrdiff_t spline_ind = wip_spline - map_data.splines.data();
        rendered_map.spline_renderer.dirty_splines.insert(spline_ind);

        if(wip_spline->spline_type == spline_t::bezier)
        {
            ASSERT(wip_spline->control_nodes.size() >= 2, "");
            wip_spline->control_nodes.rbegin()[1] = position; // control curve behind
            wip_spline->control_nodes.rbegin()[0] = position;// control for curve ahead
        }
    }

    void editor_t::update_WIP_control_nodes(glm::vec2 const& position)
    {
        ASSERT(wip_spline, "");
        if(wip_spline->spline_type == data::spline_t::linear)
        {
            return;
        }

        ASSERT(wip_spline->nodes.size() >= 2, "");
        auto const& node = wip_spline->nodes.rbegin()[1]; //grab second to last (last will be WIP node);

        // all nodes have two control nodes except the first and last nodes
        // the second control node for the last line node will be removed in finish_spline
        ASSERT(wip_spline->control_nodes.size() >= 4, "");
        wip_spline->control_nodes.rbegin()[3] = node.position - (position - node.position);  // control curve behind
        wip_spline->control_nodes.rbegin()[2] = position; // control for curve ahead
    }

    void editor_t::add_node_to_wip_spline(data::line_node_t node)
    {
        ASSERT(wip_spline, "cannot add a spline node when not currently constructing one");
        LOG("adding spline node to (%f,%f)", node.position.x, node.position.y);

        *wip_spline_node = node;
        wip_spline->nodes.push_back(node);
        wip_spline_node = &(wip_spline->nodes.back());

        if(wip_spline->spline_type == spline_t::bezier)
        {
            //these control points belong to the new WIP node
            wip_spline->control_nodes.emplace_back(wip_spline_node->position);
            wip_spline->control_nodes.emplace_back(wip_spline_node->position);
        }
    }

    ///FIXME: use the new spline_t::loops property
    void editor_t::finish_spline(bool spline_loops)
    {
        ASSERT(wip_spline, "finishing a spline that hasnt even started");

        if(!spline_loops)
        {
            wip_spline->nodes.pop_back(); //kill the WIP node

            if(wip_spline->control_nodes.size() > 0)
            {
                ASSERT(wip_spline->control_nodes.size() >= 2, "");
                wip_spline->control_nodes.pop_back(); //and its control nodes
                wip_spline->control_nodes.pop_back(); //
            }
        }
        else
        {
            wip_spline->nodes.back().position = wip_spline->nodes[0].position;
        }

        if(wip_spline->nodes.size() < 2)
        {
            cancel_spline(); // if clicking on the one and only node, just cancel
            return;
        }

        // //remove the terminating control node (if this spline has control nodes)
        // if(wip_spline->control_nodes.size() > 0)
        // {
        //     wip_spline->control_nodes.pop_back();
        // }

        LOG("finished a%s spline", spline_loops ? " looping" : "");

        ASSERT(wip_spline->control_nodes.size() >= 0, "apparently control_nodes might have a negtaive length, which I thought was impossible");

        auto cmd = make_unique<add_spline_action_t>(map_data, *wip_spline);
        push_action(std::move(cmd));

        wip_spline = nullptr;
        wip_spline_node = nullptr;
    }

    void editor_t::cancel_spline()
    {
        map_data.splines.pop_back();

        wip_spline = nullptr;
        wip_spline_node = nullptr;
    }



    void editor_t::cancel_action()
    {
        switch(current_tool)
        {
            case select:
            {
                deselect_all();
                break;
            }
            case terrain_paint:
            {
                break;
            }
            case place_objects:
            {
                break;
            }
            case place_splines:
            {
                cancel_spline();
                break;
            }
            default: break;
        };
    }


    data::map_object_t editor_t::__placeable_object(glm::vec2 position)
    {
        auto const& atlas_entries = map_data.objects_atlas->atlas_entries;
        ASSERT(current_object_id < atlas_entries.size(), "object ID does not exist in atlas");
        auto const& atlas_entry = atlas_entries[current_object_id];
        glm::vec2 size = glm::vec2(atlas_entry.size_px) * units_per_px;

        data::map_object_t obj{current_object_id, position, size / 2.0f, glm::vec4(1), glm::vec2(1,1), 0.0f};

        return obj;
    }

    void editor_t::enable_wip_object()
    {
        WARN_IF(current_tool != place_objects, "Enabling WIP object when using a different tool");
        wip_object().color = selection_overlay_color;
    }

    void editor_t::disable_wip_object()
    {
        wip_object().color = color_t(0.0f);
    }



    /// Object Selection
    object_selection_t::object_selection_t(editor_t& _e)
    : base_selection_t{vec2{}, vec2{},_e}
    {
    }

    bool object_selection_t::operator ==(object_selection_t const& rhs) const
    {
        return object_indices == rhs.object_indices && spline_indices == rhs.spline_indices;
    }

    bool object_selection_t::operator !=(object_selection_t const& rhs) const
    {
        return !(*this == rhs);
    }

    bool object_selection_t::add_object_index(size_t obj_ind)
    {
        auto x = object_indices.insert(obj_ind);
        recalc_bounds();

        //second value of pair returned by insert is a bool that is true
        //if the insertion took place
        return x.second;
    }

    bool object_selection_t::remove_object_index(size_t obj_ind)
    {
        // not 100% sure if it erases the object at this position, or with this key
        // I think it's with the key
        auto n = object_indices.erase(obj_ind);
        recalc_bounds();

        return n > 0;
    }

    void object_selection_t::clear_selection()
    {
        object_indices.clear();
        recalc_bounds();
    }

    void object_selection_t::recalc_bounds()
    {
        upper_bound = vec2(numeric_limits<float>::min());
        lower_bound = vec2(numeric_limits<float>::max());

        for(auto const& obj_ind : object_indices)
        {
            auto const& obj = editor.map_data.objects[obj_ind];

            upper_bound.x = std::max(upper_bound.x, obj.position.x + obj.size_d2.x);
            upper_bound.y = std::max(upper_bound.y, obj.position.y + obj.size_d2.y);

            lower_bound.x = std::min(lower_bound.x, obj.position.x - obj.size_d2.x);
            lower_bound.y = std::min(lower_bound.y, obj.position.y - obj.size_d2.y);
        }
    }



    /// Spline Node Selection
    spline_node_selection_t::spline_node_selection_t(editor_t& _editor)
    : base_selection_t{vec2{}, vec2{},_editor}
    {
    }

    bool spline_node_selection_t::add_node_index(spline_index_t _si, spline_node_index_t _ni)
    {
        auto x = node_indices[_si].insert(_ni);
        recalc_bounds();
        return x.second;
    }

    bool spline_node_selection_t::remove_node_index(spline_index_t _si, spline_node_index_t _ni)
    {
        bool was_removed = false;
        if(node_indices.count(_si) > 0)
        {
            auto n = node_indices[_si].erase(_ni);
            was_removed = n > 0;
        }
        
        recalc_bounds();
        return was_removed;
    }

    bool spline_node_selection_t::add_all_nodes_from_spline(spline_index_t _si)
    {
        bool nodes_were_added = false;
        auto& node_set = node_indices[_si];

        ASSERT(editor.map_data.splines.size() >= _si, "no spline at index %zu", _si);
        auto const& spline = editor.map_data.splines[_si];
        for(size_t i = 0; i < spline.nodes.size(); ++i)
        {
            auto x = node_set.insert(i);
            nodes_were_added |= x.second;
        }

        recalc_bounds();
        return nodes_were_added;
    }

    bool spline_node_selection_t::remove_all_nodes_from_spline(spline_index_t _si)
    {
        bool was_removed = false;

        if(node_indices.count(_si) > 0)
        {
            auto& node_set = node_indices[_si];
            ASSERT(editor.map_data.splines.size() >= _si, "no spline at index %zu", _si);
            auto const& spline = editor.map_data.splines[_si];
            for(size_t i = 0; i < spline.nodes.size(); ++i)
            {
                auto n = node_set.erase(i);
                was_removed |= n > 0;
            }
        }

        recalc_bounds();
        return was_removed;
    }

    void spline_node_selection_t::clear_selection()
    {
        node_indices.clear();
        recalc_bounds();
    }

    void spline_node_selection_t::recalc_bounds()
    {
        
    }

}
}
}
