#include "stdafx.h"
#include "input.h"

#include "asdf_multiplat/main/mouse.h"

#include "editor.h"

namespace asdf
{
    using namespace input;

namespace hexmap 
{
namespace editor
{

    input_handler_t::input_handler_t(editor_t& _editor)
    : input::hex_map_input_t(_editor.rendered_map.get(), _editor.rendered_map->camera)
    , editor(_editor)
    {
    }

    //TODO: move this into an asdf_multiplat header
    constexpr bool is_sdl_mouse_event(SDL_Event* event)
    {
        return event->type == SDL_MOUSEMOTION
            || event->type == SDL_MOUSEBUTTONDOWN
            || event->type == SDL_MOUSEBUTTONUP
            || event->type == SDL_MOUSEWHEEL
            ;
    }

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


    bool input_handler_t::on_event(SDL_Event* event)
    {
        input::hex_map_input_t::on_event(event);


        if(is_sdl_mouse_event(event))
        {
            return on_mouse_event(event);
        }
        else if(is_sdl_keyboard_event(event))
        {
            if(event->key.type == SDL_KEYDOWN)
                on_key_down(event->key.keysym);
            // else
            //     on_key_up(event->key.keysym);
        }        

        return false;
    }

    bool input_handler_t::on_mouse_event(SDL_Event* event)
    {
        auto mw = world_coords(mouse_input->mouse_position);
        auto hx = world_to_hex_coord(mw);

        //LOG_IF(mouse_input->mouse_button_state(mouse_left)
        //    , "mw: %.2f, %.2f   hx: %i, %i", mw.x, mw.y, hx.x, hx.y);


        if(event->type == SDL_MOUSEBUTTONUP)
        {
            dragging_object = false;
        }


        switch(editor.current_tool)
        {
            case editor_t::select:
            {
                if(event->type == SDL_MOUSEBUTTONDOWN)
                {
                    editor.select_object_at(mw);
                    dragging_object = true;
                }
                else if(event->type == SDL_MOUSEMOTION)
                {
                    if(dragging_object)
                    {
                        //editor.selected_object().position = mw;
                    }
                }
                else if(event->type == SDL_MOUSEBUTTONUP)
                {
                    //editor.action_stack.push(std::make_unique<move_map_object_t>());
                }

                break;
            }

            case editor_t::terrain_paint:
            {
                if(event->type == SDL_MOUSEBUTTONDOWN)
                {
                    editor.paint_terrain_start();
                }
                else if(event->type == SDL_MOUSEBUTTONUP)
                {
                    editor.paint_terrain_end();
                }
                else
                {
                    if(mouse_input->mouse_button_state(mouse_left))
                    {
                        editor.paint_terrain_at_coord(hx);
                    }
                    else if(mouse_input->mouse_button_state(mouse_right))
                    {
                    }
                }

                break;
            }

            case editor_t::place_objects:
            {
                if(event->type == SDL_MOUSEBUTTONDOWN)
                {
                    //todo: handle snapping to hex regions

                    editor.place_object(mw);
                }
                break;
            }

            case editor_t::place_splines:
            {
                break;
            }
        }


        return false;
    }

    void input_handler_t::on_key_down(SDL_Keysym keysm)
    {
        auto& key = keysm.sym;

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

            LOG("current tile_id: %d", current_tile_id);
            LOG("current object_id: %d", current_object_id);
        }
    }

}
}
}