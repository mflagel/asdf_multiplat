#pragma once

#include <memory>

#include "main/hexmap.h"
#include "editor/main/input.h"

namespace asdf {
namespace hexmap {
namespace editor
{

    struct editor_t : hexmap_t
    {
        std::unique_ptr<input_handler_t> input;

        editor_t();

        void init();

        void on_event(SDL_Event*) override;
    };

}
}
}