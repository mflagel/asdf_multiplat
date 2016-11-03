#include "editor.h"

#include "asdf_multiplat/data/content_manager.h"


using namespace std;
using namespace glm;

namespace asdf {
namespace hexmap {
namespace editor
{

    const color_t selection_overlay_color = color_t(1.0, 1.0, 1.0, 0.5f);

    editor_t::editor_t()
    : hexmap_t()
    , action_stack(*this)
    {}

    void editor_t::init()
    {
        hexmap_t::init();

        input = make_unique<input_handler_t>(*this);
    }

    void editor_t::render()
    {
        hexmap_t::render();

        if(is_object_selected())
        {
            //TODO: draw selection overlay
            auto const& sel_obj = map_data.objects[selected_object_index];

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
        hexmap_t::on_event(event);

        input->on_event(event);
    }


    void editor_t::set_tool(editor_t::tool_type_e const& new_tool)
    {
        //todo: handle state transitions if necessary

        current_tool = new_tool;

        LOG("current tool: %s", tool_type_strings[current_tool]);
    }


    /// Selection
    void editor_t::select_object(size_t object_index)
    {
        selected_object_index = object_index;
    }

    size_t editor_t::select_object_at(glm::vec2 position)
    {
        select_object(map_data.object_index_at(position));
        LOG("selected object: %zu", selected_object_index);
        return selected_object_index;
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
        action_stack.push(make_unique<paint_tiles_action_t>
            ( map_data.hex_grid
            , std::move(painted_terrain_coords)
            , current_tile_id
            )
        );

        painted_terrain_coords.clear(); //pretty sure its empty anyway due to std::move
    }


    /// Map Objects
    void editor_t::place_object(glm::vec2 position)
    {
        auto const& atlas_entries = rendered_map->ojects_atlas->atlas_entries;
        ASSERT(current_object_id < atlas_entries.size(), "object ID does not exist in atlas");
        auto const& atlas_entry = atlas_entries[current_object_id];
        glm::vec2 size = glm::vec2(atlas_entry.size_px) * units_per_px;

        data::map_object_t obj{current_object_id, position, size, glm::vec4(1), glm::vec2(1,1), 0.0f};
        map_data.objects.push_back(std::move(obj));
    }

}
}
}