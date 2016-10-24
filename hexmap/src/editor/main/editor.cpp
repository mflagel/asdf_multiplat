#include "editor.h"

using namespace std;
using namespace glm;

namespace asdf {
namespace hexmap {
namespace editor
{

    editor_t::editor_t()
    : hexmap_t()
    , action_stack(*this)
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


    void editor_t::paint_terrain_start()
    {
        painted_terrain_coords.clear();
    }

    bool editor_t::paint_terrain_at_coord(glm::ivec2 coord)
    {
        if(map_data.hex_grid.is_in_bounds(coord))
        {
            auto& cell = map_data.hex_grid.cell_at(coord);
            cell.tile_id = current_tile_id;
            return true;
        }
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

    void editor_t::place_object(glm::vec2 position)
    {
        data::map_object_t obj{current_object_id, position};
        map_data.objects.push_back(std::move(obj));
    }

}
}
}