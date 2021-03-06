#include "stdafx.h"
#include "input.h"

#include <asdfm/main/mouse.h>
#include <asdfm/main/input_sdl.h>

#include "data/hex_util.h"

#include "editor.h"


using namespace std;
using namespace glm;

namespace asdf {
namespace hexmap {
namespace editor
{

    input_handler_t::input_handler_t(editor_t& _editor)
    : editor(_editor)
    {
    }


    glm::vec2 input_handler_t::world_coords(glm::ivec2 screen_coords) const
    {
        return vec2(editor.rendered_map.camera.screen_to_world_coord(vec2(screen_coords)));
    }

    glm::vec2 input_handler_t::get_snap_position(glm::vec2 world_coord) const
    {
        auto nearest_snap = nearest_snap_point(world_coord, editor.snap_mode);
        if(glm::abs(glm::distance(nearest_snap, world_coord)) <= editor.snap_threshold)
            return nearest_snap;

        return world_coord;
    }

    bool input_handler_t::on_mouse_down(mouse_button_event_t& event)
    {
         vec2 mw = world_coords(event.mouse_state.mouse_position);
        ivec2 hx = world_to_hex_coord(mw);
         vec2 snap_pos = get_snap_position(mw);


        switch(editor.current_tool)
        {
            case editor_t::select:
            {
                auto const& sel_objs = editor.object_selection;
                vec2 const& ub = sel_objs.upper_bound;
                vec2 const& lb = sel_objs.lower_bound;
                
                if(util::CheckBounds(mw.x, mw.y
                                   , lb.x, lb.y
                                   , ub.x, ub.y))
                {
                    editor.drag_type = editor_t::drag_selected_items;
                }
                else
                {
                    editor.drag_type = editor_t::drag_selection_box;
                    editor.start_drag_selection(mw);
                }

                break;
            }

            case editor_t::terrain_paint:
            {
                editor.paint_terrain_start();
                editor.paint_terrain_at_coord(hx);
                return true;
            }

            case editor_t::place_objects:
            {
                editor.place_object(snap_pos);
                return true;
            }

            case editor_t::place_splines:
            {
                if(event.button == mouse_left)
                {
                    editor.spline_click(snap_pos);
                }
                else if(event.button == mouse_right)
                {
                    if(editor.is_placing_spline())
                    {
                        editor.finish_spline();
                    }
                }

                break;
            }

            case editor_t::num_tool_types: break;
        }

        return false;
    }

    bool input_handler_t::on_mouse_up(mouse_button_event_t& event)
    {
        auto mw = world_coords(event.mouse_state.mouse_position);
        auto hx = world_to_hex_coord(mw);

        switch(editor.current_tool)
        {
            case editor_t::select:
            {
                if(!event.mouse_state.is_dragging(mouse_left))
                {
                    if(handle_click_selection(event, mw))
                        event.handled = true;
                    break;
                }

                switch(editor.drag_type)
                {
                    case editor_t::drag_type_none:
                    {

                    }
                    case editor_t::drag_selection_box:
                    {
                        editor.end_drag_selection(mw);
                        break;
                    }
                    case editor_t::drag_selected_items:
                    {
                        break;
                    }
                };

                break;
            }

            case editor_t::terrain_paint:
            {
                editor.paint_terrain_at_coord(hx);
                editor.paint_terrain_end();
                return true;
            }

            case editor_t::place_objects:
            {
                break;
            }

            case editor_t::place_splines:
            {
                break;
            }

            case editor_t::num_tool_types: break;
        }

        editor.drag_type = editor_t::drag_type_none;

        return false;
    }

    bool input_handler_t::on_mouse_move(mouse_motion_event_t& event)
    {
         vec2 mw = world_coords(event.mouse_state.mouse_position);
        ivec2 hx = world_to_hex_coord(mw);
         vec2 snap_pos = get_snap_position(mw);

        switch(editor.current_tool)
        {
            case editor_t::select:
            {
                break;
            }

            case editor_t::terrain_paint:
            {
                editor.brush_pos = hex_to_world_coord(hx);

                break;
            }

            case editor_t::place_objects:
            {
                editor.brush_pos = snap_pos;
                editor.wip_object().position = snap_pos;
                break;
            }

            case editor_t::place_splines:
            {
                if(editor.is_placing_spline())
                {
                    editor.update_WIP_node(snap_pos);
                }
                break;
            }

            case editor_t::num_tool_types: break;
        }

        return false;
    }

    bool input_handler_t::on_mouse_drag(mouse_motion_event_t& event)
    {
        auto mw = world_coords(event.mouse_state.mouse_position);
        auto hx = world_to_hex_coord(mw);

        switch(editor.current_tool)
        {
            case editor_t::select:
            {
                switch(editor.drag_type)
                {
                    case editor_t::drag_type_none:
                    {
                    }
                    case editor_t::drag_selection_box:
                    {
                        editor.update_drag_selection(mw);
                        break;
                    }
                    case editor_t::drag_selected_items:
                    {
                        break;
                    }
                };

                break;
            }

            case editor_t::terrain_paint:
            {
                editor.brush_pos = hex_to_world_coord(hx);

                /// If the user is dragging the mouse
                /// paint hexes along the line they have dragged
                /// otherwise it will only paint one hex per update
                /// which looks terrible if their performance is bad
                if(event.mouse_state.mouse_button_state(mouse_left))
                {
                    vec2 mw_begin = world_coords(event.mouse_state.mouse_prev_position);
                    vec2 mw_end = world_coords(event.mouse_state.mouse_position);

                    editor.paint_terrain_along_line(mw_begin, mw_end);

                    return true;
                }

                break;
            }

            case editor_t::place_objects:
            {
                editor.brush_pos = mw;

                break;
            }

            case editor_t::place_splines:
            {
                if(editor.is_placing_spline())
                {
                    if(event.mouse_state.is_dragging(mouse_left))
                    {
                        editor.update_WIP_control_nodes(mw);
                    }

                    editor.update_WIP_node(mw);
                }
                break;
            }

            case editor_t::num_tool_types: break;
        }

        return false;
    }

    bool input_handler_t::on_mouse_wheel(mouse_wheel_event_t& event)
    {
        ASDF_UNUSED(event);

        return false;
    }

    void input_handler_t::on_key_down(SDL_Keysym keysm)
    {
        auto& key = keysm.sym;

        modifier_keys = modifier_keys_from_sdl2_event(keysm, true);

        bool is_shft = (modifier_keys & KMOD_SHIFT)>0;
        bool is_ctrl = (modifier_keys & KMOD_CTRL)>0;
        bool is_alt  = (modifier_keys & KMOD_ALT)>0;


        bool mod_ctrl_only = modifier_keys == KMOD_LCTRL
                          || modifier_keys == KMOD_RCTRL;

        if(key == SDLK_p)
        {
            LOG("Key Mods: %s %s %s %s"
                , (modifier_keys & KMOD_SHIFT)>0 ? "S" : "-"
                , (modifier_keys & KMOD_CTRL)>0  ? "C" : "-"
                , (modifier_keys & KMOD_ALT)>0   ? "A" : "-"
                , (modifier_keys & KMOD_GUI)>0   ? "^" : "-"
            );
        }

        //modifier key combos first
        if(mod_ctrl_only)
        {
            switch(key)
            {
                case SDLK_n:
                    editor.new_map_action("", editor.map_data.hex_grid.size);
                    return;

                //save on ctrl+s
                case SDLK_s:
                    editor.save_action("test_map.hxm");
                    return;

                //load on ctrl+o
                case SDLK_o:
                    editor.load_action("test_map.hxm");
                    return;

                case SDLK_z:
                    editor.undo();
                    return;

                case SDLK_y:
                    editor.redo();
                    return;

                default: return;
            }
        }
        else if(is_ctrl && is_shft)
        {
            switch(key)
            {
                case SDLK_z:
                    editor.redo();
                    return;
            }
        }
        else if ((keysm.mod & KMOD_ALT) > 0)
        {
            switch(key)
            {
                case SDLK_1: editor.set_tool(editor_t::select);            break;
                case SDLK_2: editor.set_tool(editor_t::terrain_paint);     break;
                case SDLK_3: editor.set_tool(editor_t::place_objects);      break;
                case SDLK_4: editor.set_tool(editor_t::place_splines);      break;
            }
        }
        else
        {
            if(key >= SDLK_0 && key <= SDLK_9)
            {
                ASSERT(uint64_t(key) >= uint64_t(SDLK_0), "");
                uint64_t num_from_key = uint64_t(key) - uint64_t(SDLK_0);

                switch(editor.current_tool)
                {
                    case editor_t::terrain_paint:
                    {
                        // make it 1-indexed such that pressing 1 selects the
                        // 'first'(zero'th) tile
                        editor.set_current_tile_id(num_from_key - 1);
                        break;
                    }

                    case editor_t::place_objects:
                    {
                        editor.set_current_object_id(num_from_key);
                        break;
                    }
                    case editor_t::place_splines:
                    {
                        num_from_key += 11 * (num_from_key == 0);
                        --num_from_key;

                        auto interp_type = static_cast<data::spline_t::interpolation_e>(num_from_key);
                        editor.set_current_spline_interpolation(interp_type);
                        break;
                    }
                }
            }


            switch(key)
            {
                case sdl2_input_map[cancel_action]:
                    editor.cancel_action();
                    break;

                case sdl2_input_map[set_tool_select]:
                    editor.set_tool(editor_t::select);
                    break;
                case sdl2_input_map[set_tool_terrain_paint]:
                    editor.set_tool(editor_t::terrain_paint);
                    break;
                case sdl2_input_map[set_tool_place_object]:
                    editor.set_tool(editor_t::place_objects);
                    break;
                case sdl2_input_map[set_tool_place_spline]:
                    editor.set_tool(editor_t::place_splines);
                    break;

                case SDLK_EQUALS: //plus
                    editor.new_node_style.thickness += 0.2f;
                    LOG("spline thickness: %0.3f", editor.new_node_style.thickness);
                    break;
                case SDLK_MINUS:
                    editor.new_node_style.thickness -= 0.2f;
                    LOG("spline thickness: %0.3f", editor.new_node_style.thickness);
                    break;

                default: return;
            }
        }
    }

    void input_handler_t::on_key_up(SDL_Keysym keysm)
    {
        modifier_keys = modifier_keys_from_sdl2_event(keysm, false);

        // LOG("Key Mods: %s %s %s %s"
        //     , (modifier_keys & KMOD_SHIFT)>0 ? "S" : "-"
        //     , (modifier_keys & KMOD_CTRL)>0  ? "C" : "-"
        //     , (modifier_keys & KMOD_ALT)>0   ? "A" : "-"
        //     , (modifier_keys & KMOD_GUI)>0   ? "^" : "-"
        // );
    }


    bool input_handler_t::handle_click_selection(mouse_button_event_t& event, vec2 const& mw)
    {
        size_t obj_ind = editor.map_data.object_index_at(mw);

        if(obj_ind != nullindex)
        {
            switch(modifier_keys)
            {
                case KMOD_LSHIFT: [[fallthrough]];
                case KMOD_RSHIFT:
                    editor.select_object(obj_ind);
                    break;

                case KMOD_LALT: [[fallthrough]];
                case KMOD_RALT:
                    editor.deselect_object(obj_ind);
                    break;

                case 0:
                    editor.deselect_all();
                    editor.select_object(obj_ind);
                    break;

                default:
                    return false;
            };

            return true;
        }
        else if(modifier_keys == 0)
        {
            editor.deselect_all();
            return true;
        }

        return false;
    }

}
}
}
