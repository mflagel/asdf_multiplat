#include "editor.h"



using namespace std;
using namespace glm;

namespace asdf {
namespace hexmap {
namespace editor
{

    editor_t::editor_t()
    : hexmap_t()
    {}

    void editor_t::init()
    {
        hexmap_t::init();

        input = make_unique<input_handler_t>(*this);
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


    bool editor_t::paint_at_coord(glm::ivec2 coord)
    {
        if(map_data.hex_grid.is_in_bounds(coord))
        {
            auto& cell = map_data.hex_grid.cell_at(coord);
            cell.tile_id = current_tile_id;
            return true;
        }
    }

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