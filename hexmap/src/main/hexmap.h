#pragma once

#include "asdf_multiplat/main/asdf_multiplat.h"
#include "asdf_multiplat/utilities/camera.h"
#include "asdf_multiplat/main/input_controller.h"
#include "asdf_multiplat/utilities/utilities.h"

#include "data/hex_map.h"
#include "ui/hex_map.h"

namespace asdf
{
namespace hexmap
{
    struct hexmap_t : asdf_specific_t
    {
        data::hex_map_t map_data;
        //std::unique_ptr<ui::hex_map_t> rendered_map;
        ui::hex_map_t rendered_map;
        ui::hex_map_t::render_flags_e map_render_flags = ui::hex_map_t::everything;

        hexmap_t();
        virtual ~hexmap_t() = default;

        virtual void init() override;

        virtual void resize(uint32_t w, uint32_t h) override;

        virtual void update(float dt) override;
        virtual void render() override;

        virtual void on_event(SDL_Event*) override;
    };
}
}