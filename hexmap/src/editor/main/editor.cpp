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

        input = make_unique<input_handler_t>(hex_map.get(), hex_map->camera);
    }

    void editor_t::on_event(SDL_Event* event)
    {
        hexmap_t::on_event(event);

        input->on_event(event);
    }

}
}
}