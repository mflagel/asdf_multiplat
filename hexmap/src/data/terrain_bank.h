#pragma once

#include <string>
#include <vector>

#include "asdf_multiplat/data/texture_bank.h"

namespace asdf {
namespace hexmap {
namespace data
{
    struct terrain_bank_t : asdf::data::texture_bank_t
    {
        using asdf::data::texture_bank_t::texture_bank_t;

        // std::vector<glm::vec4> terrain_colors;

    private:
        // framebuffer_t colors_fbo;

    public:

        void load_from_file(std::experimental::filesystem::path const& filepath);
        //void rebuild_colors();
    };
}
}
}