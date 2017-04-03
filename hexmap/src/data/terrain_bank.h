#pragma once

#include <string>
#include <vector>

#include "asdf_multiplat/data/texture_bank.h"
#include "asdf_multiplat/utilities/cjson_utils.hpp"

namespace asdf {
namespace hexmap {
namespace data
{
    struct terrain_bank_t : asdf::data::texture_bank_t
    {
        struct entry_t
        {
            std::string name;
            std::string asset;

            void from_JSON(cJSON*);
        };

        using asdf::data::texture_bank_t::texture_bank_t;

        std::vector<std::string> asset_names;
        std::vector<glm::vec4> terrain_colors;

    private:
        framebuffer_t colors_fbo;

    public:

        void load_from_file(std::string const& filepath);
        void rebuild_colors();
    };
}
}
}