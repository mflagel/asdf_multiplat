#include "stdafx.h"
#include "texture_atlas.h"

#include "texture.h"

#include "utilities/utilities.h"
#include "utilities/cjson_utils.hpp"

using namespace std;
using namespace glm;

namespace asdf
{
    using namespace util;

namespace data
{

    texture_atlas_t::texture_atlas_t(std::string const& json_filepath)
    {
        import_from_json(json_filepath);
    }

    // Designed to import from TexturePacker, because that's a good a standard as any
    void texture_atlas_t::import_from_json(std::string const& filepath)
    {
        string json_str = read_text_file(filepath);
        cJSON* root = cJSON_Parse(json_str.c_str());
        ASSERT(root, "Error loading imported textures json file");


        auto meta_json = cJSON_GetObjectItem(root, "meta");
        auto atlas_texture_filename = cJSON_GetObjectItem(meta_json, "image")->valuestring;

        //atlas texture is in the same folder, so just chop off the data filename from its path and append the texture filename
        std::string atlas_texture_filepath;
        auto last_slash_index = filepath.find_last_of("/\\");
        if(last_slash_index == std::string::npos)
        {
            atlas_texture_filepath = atlas_texture_filename ;
        }
        else
        {
            atlas_texture_filepath = filepath.substr(0, last_slash_index) + "/" + atlas_texture_filename;
        }

        atlas_texture = make_shared<texture_t>(atlas_texture_filepath);


        auto frames_json = cJSON_GetObjectItem(root, "frames"); //array of entries ('frames')
        size_t len = cJSON_GetArraySize(frames_json);
        for(size_t i = 0; i < len; ++i)
        {
            auto entry_json = cJSON_GetArrayItem(frames_json, i);

            auto frame_json = cJSON_GetObjectItem(entry_json, "frame");

            entry_t entry{string(cJSON_GetObjectItem(entry_json, "filename")->valuestring)
                        , uvec2(cJSON_GetObjectItem(frame_json, "x")->valueint, cJSON_GetObjectItem(frame_json, "y")->valueint)
                        , uvec2(cJSON_GetObjectItem(frame_json, "w")->valueint, cJSON_GetObjectItem(frame_json, "h")->valueint)
                        };

            atlas_entries.push_back(std::move(entry));
        }
        
        cJSON_Delete(root);
    }
}
}