#include "stdafx.h"
#include "input.h"

#include "asdf_multiplat/main/mouse.h"
#include "asdf_multiplat/main/input_sdl.h"

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


    glm::vec2 input_handler_t::world_coords(glm::ivec2 screen_coords)
    {
        return vec2(editor.rendered_map->camera.screen_to_world_coord(vec2(screen_coords)));
    }

    bool input_handler_t::on_mouse_down(mouse_button_event_t& event)
    {
        auto mw = world_coords(event.mouse_state.mouse_position);
        auto hx = world_to_hex_coord(mw);

        switch(editor.current_tool)
        {
            case editor_t::select:
            {
                //todo: start select box dragging

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
                editor.place_object(mw);
                return true;
            }

            case editor_t::place_splines:
            {
                if(event.button == mouse_left)
                {
                    editor.spline_click(mw);
                }
                else if(event.button == mouse_right)
                {
                    editor.cancel_action();
                }
                
                break;
            }

            case editor_t::num_tool_types: break;
        }

        return false;
    }

    bool input_handler_t::handle_click_selection(mouse_button_event_t& event, vec2 const& mw)
    {
        size_t obj_ind = editor.map_data.object_index_at(mw);

        if(obj_ind != size_t(-1))
        {
            switch(modifier_keys)
            {
                case KMOD_LSHIFT: [[fallthrough]]
                case KMOD_RSHIFT:
                    editor.select_object(obj_ind);
                    break;

                case KMOD_LALT: [[fallthrough]]
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

    bool input_handler_t::on_mouse_up(mouse_button_event_t& event)
    {
        auto mw = world_coords(event.mouse_state.mouse_position);
        auto hx = world_to_hex_coord(mw);

        switch(editor.current_tool)
        {
            case editor_t::select:
            {
                if(!event.mouse_state.is_dragging())
                {
                    if(handle_click_selection(event, mw))
                        return true;
                }

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

        return false;
    }

    bool input_handler_t::on_mouse_move(mouse_motion_event_t& event)
    {
        auto mw = world_coords(event.mouse_state.mouse_position);
        auto hx = world_to_hex_coord(mw);

        switch(editor.current_tool)
        {
            case editor_t::select:
            {
                break;
            }

            case editor_t::terrain_paint:
            {
                if(event.mouse_state.mouse_button_state(mouse_left))
                {
                    editor.paint_terrain_at_coord(hx);
                    return true;
                }

                break;
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

        LOG("Key Mods: %s %s %s %s"
            , (modifier_keys & KMOD_SHIFT)>0 ? "S" : "-"
            , (modifier_keys & KMOD_CTRL)>0  ? "C" : "-"
            , (modifier_keys & KMOD_ALT)>0   ? "A" : "-"
            , (modifier_keys & KMOD_GUI)>0   ? "^" : "-"
        );


        //bool mod_ctrl_only = keysm.mod == KMOD_LCTRL || keysm.mod == KMOD_RCTRL;
        bool mod_ctrl_only = (keysm.mod & KMOD_CTRL) > 0;

        //modifier key combos first
        if(mod_ctrl_only)
        {
            switch(key)
            {
                //save on ctrl+s
                case SDLK_s:
                    editor.save_action();
                    return;

                //load on ctrl+o
                case SDLK_o:
                    editor.load_action();
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
            auto& current_tile_id   = editor.current_tile_id;
            auto& current_object_id = editor.current_object_id;

            switch(key)
            {
                case SDLK_1: current_tile_id = 1; break;
                case SDLK_2: current_tile_id = 2; break;
                case SDLK_3: current_tile_id = 3; break;
                case SDLK_4: current_tile_id = 4; break;
                case SDLK_5: current_tile_id = 5; break;
                case SDLK_6: current_tile_id = 6; break;
                case SDLK_7: current_tile_id = 7; break;
                case SDLK_8: current_tile_id = 8; break;
                case SDLK_9: current_tile_id = 9; break;
                case SDLK_0: current_tile_id = 0; break;

                case SDLK_KP_1: current_object_id = 1; break;
                case SDLK_KP_2: current_object_id = 2; break;
                case SDLK_KP_3: current_object_id = 3; break;
                case SDLK_KP_4: current_object_id = 4; break;
                case SDLK_KP_5: current_object_id = 5; break;
                case SDLK_KP_6: current_object_id = 6; break;
                case SDLK_KP_7: current_object_id = 7; break;
                case SDLK_KP_8: current_object_id = 8; break;
                case SDLK_KP_9: current_object_id = 9; break;
                case SDLK_KP_0: current_object_id = 0; break;

                case SDLK_KP_PERIOD: current_object_id = -1; break;


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

                default: return;
            }

            LOG("current tile_id: %ld", current_tile_id);
            LOG("current object_id: %ld", current_object_id);
        }
    }

    void input_handler_t::on_key_up(SDL_Keysym keysm)
    {
        modifier_keys = modifier_keys_from_sdl2_event(keysm, false);

        LOG("Key Mods: %s %s %s %s"
            , (modifier_keys & KMOD_SHIFT)>0 ? "S" : "-"
            , (modifier_keys & KMOD_CTRL)>0  ? "C" : "-"
            , (modifier_keys & KMOD_ALT)>0   ? "A" : "-"
            , (modifier_keys & KMOD_GUI)>0   ? "^" : "-"
        );
    }

}
}
}