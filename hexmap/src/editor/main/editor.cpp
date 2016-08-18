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

}
}
}