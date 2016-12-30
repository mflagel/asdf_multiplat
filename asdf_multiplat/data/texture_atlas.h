#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

//#include "texture.h"

namespace asdf
{
    struct texture_t;

namespace data
{

    /*
        Currently stores with 0,0 as the top left, but might change that
        to have 0,0 as the bottom left to be consistent with texture_bank_t
    */
    struct texture_atlas_t
    {
        struct entry_t
        {
            std::string filename;

            glm::uvec2 top_left_px;
            glm::uvec2 size_px;
        };

        std::vector<entry_t> atlas_entries;

        std::string texture_filepath;
        std::shared_ptr<texture_t> atlas_texture;

        texture_atlas_t(std::string const& json_filepath);

        void import_from_json(std::string const& filepath);
    };

}
}