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
        enum tool_type_e
        {
              select = 0
            , terrain_paint
            , place_object
            , place_spline
            , num_tool_types
        };

        tool_type_e current_tool = terrain_paint;

        std::unique_ptr<input_handler_t> input;

        editor_t();
        void init() override;

        void save_action();
        void load_action();

        void on_event(SDL_Event*) override;

        void set_tool(tool_type_e const& new_tool);
        
        bool paint_at_coord(glm::ivec2 coord);
    };


    constexpr std::array<const char*, editor_t::num_tool_types> tool_type_strings =
    {
          "select"
        , "terrain_paint"
        , "place_object"
        , "place_spline"
    };

}
}
}