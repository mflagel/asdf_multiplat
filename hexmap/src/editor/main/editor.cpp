#include "editor.h"

#include <limits>

#include "asdf_multiplat/data/content_manager.h"


using namespace std;
using namespace glm;

namespace asdf {
namespace hexmap {
namespace editor
{

    const color_t selection_overlay_color = color_t(1.0, 1.0, 1.0, 0.5f);


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
    {}

    void editor_t::init()
    {
        hexmap_t::init();

        input = make_unique<input_handler_t>(*this);
        app.mouse_state.thing = input.get();
    }

    void editor_t::render()
    {
        hexmap_t::render();

        for(auto const& sel_obj_ind : object_selection.object_indices)
        {
            auto const& sel_obj = map_data.objects[sel_obj_ind];

            auto& spritebatch = rendered_map->spritebatch;
            auto const& shader = rendered_map->shader;
            spritebatch.begin(shader->view_matrix, shader->projection_matrix);

            auto const& pixel_texture = Content.textures["pixel"];
            auto const& obj_size_px = rendered_map->ojects_atlas->atlas_entries[sel_obj.id].size_px;
            auto scale = vec2(obj_size_px) / pixel_texture->get_size(); //scale overlay texture to match object texture size
            auto sprite_scale = scale * sel_obj.scale / glm::vec2(px_per_unit);

            spritebatch.draw(pixel_texture, sel_obj.position, selection_overlay_color, sprite_scale, sel_obj.rotation);

            spritebatch.end();
        }
    }

    void editor_t::save_action()
    {
        map_data.save_to_file("test_save.hxm");
        LOG("map saved");
    }

    void editor_t::load_action()
    {
        map_data.load_from_file("test_save.hxm");
        LOG("map loaded");
    }

    bool editor_t::undo()
    {
        if(action_stack.can_undo())
        {
            action_stack.undo();
            return true;
        }

        return false;
    }

    bool editor_t::redo()
    {
        if(action_stack.can_redo())
        {
            action_stack.redo();
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
            if(event->key.type == SDL_KEYDOWN)
                input->on_key_down(event->key.keysym);
        }    
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
                break;
            }
            case place_splines:
            {
                break;
            }
        };

        current_tool = new_tool;

        LOG("current tool: %s", tool_type_strings[current_tool]);
    }


    /// Selection
    bool editor_t::select_object(size_t object_index)
    {
        ASSERT(object_index != size_t(-1), "");
        LOG("selected object: %zu", object_index);
        return object_selection.add_object_index(object_index);
    }

    bool editor_t::deselect_object(size_t object_index)
    {
        ASSERT(object_index != size_t(-1), "");
        LOG("deselected object: %zu", object_index);
        return object_selection.remove_object_index(object_index);
    }

    void editor_t::deselect_all()
    {
        object_selection.clear_selection();
    }

    bool editor_t::select_object_at(glm::vec2 position)
    {
        size_t ind = map_data.object_index_at(position);

        if(ind != size_t(-1))
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


    /// Terrain
    void editor_t::paint_terrain_start()
    {
        painted_terrain_coords.clear();
    }

    /// If the coord is inside the map, track the old tile_id of that cell
    /// and then paint it with the editor's current_tile_id
    bool editor_t::paint_terrain_at_coord(glm::ivec2 coord)
    {
        if(map_data.hex_grid.is_in_bounds(coord))
        {
            auto& cell = map_data.hex_grid.cell_at(coord);
            painted_terrain_coords.insert({coord, cell.tile_id});
            cell.tile_id = current_tile_id;
            return true;
        }

        return false;
    }

    void editor_t::paint_terrain_end()
    {
        if(!painted_terrain_coords.empty())
        {
            action_stack.push(make_unique<paint_tiles_action_t>
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
        auto const& atlas_entries = rendered_map->ojects_atlas->atlas_entries;
        ASSERT(current_object_id < atlas_entries.size(), "object ID does not exist in atlas");
        auto const& atlas_entry = atlas_entries[current_object_id];
        glm::vec2 size = glm::vec2(atlas_entry.size_px) * units_per_px;

        data::map_object_t obj{current_object_id, position, size, glm::vec4(1), glm::vec2(1,1), 0.0f};

        action_stack.push_and_execute(make_unique<add_map_object_action_t>(map_data, std::move(obj)));
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
                break;
            }
            default: break;
        };
    }

    object_selection_t::object_selection_t(editor_t& _e)
    : editor(_e)
    {
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

}
}
}