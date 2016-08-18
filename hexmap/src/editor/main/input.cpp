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
    : input::hex_map_input_t(_editor.hex_map.get(), _editor.hex_map->camera)
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
            || event->type == SDL_KEYMAPCHANGED
            ;
    }
    //--


    bool input_handler_t::on_event(SDL_Event* event)
    {
        input::hex_map_input_t::on_event(event);


        if(is_sdl_mouse_event(event))
        {
            if(mouse_input->mouse_button_state(mouse_left))
            {
                auto mw = world_coords();
                auto hx = hex_coords_from_mouse(mouse_input->mouse_position);

                LOG("mw: %.2f, %.2f   hx: %i, %i", mw.x, mw.y, hx.x, hx.y);

                if(hex_map->hex_grid.is_in_bounds(hx))
                {
                    auto& cell = hex_map->hex_grid.cell_at(hx);
                    cell.tile_id = current_tile_id;
                    return true;
                }
            }
        }
        else if(is_sdl_keyboard_event(event))
        {
            on_key_down(event->key.keysym.sym);
        }        

        return false;
    }

    void input_handler_t::on_key_down(SDL_Keycode key)
    {
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
                //TODO
                break;

            case sdl2_input_map[set_tool_select]:
                editor.set_tool(editor_t::select);
                break;
            case sdl2_input_map[set_tool_terrain_paint]:
                editor.set_tool(editor_t::terrain_paint);
                break;
            case sdl2_input_map[set_tool_place_object]:
                editor.set_tool(editor_t::place_object);
                break;
            case sdl2_input_map[set_tool_place_spline]:
                editor.set_tool(editor_t::place_spline);
                break;


        }

        //LOG("%i", key);
    }

}
}
}