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
        hex_grid.save_to_file("test_save.hxm");
        LOG("map saved");
    }

    void editor_t::load_action()
    {
        hex_grid.load_from_file("test_save.hxm");
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
        if(hex_map->hex_grid.is_in_bounds(coord))
        {
            auto& cell = hex_map->hex_grid.cell_at(coord);
            cell.tile_id = input->current_tile_id;
            return true;
        }
    }

    void editor_t::place_object(glm::vec2 position)
    {
        
    }

}
}
}