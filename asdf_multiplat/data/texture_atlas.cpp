#include "texture_atlas.h"

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
        auto atlas_texture_filepath = cJSON_GetObjectItem(meta_json, "image")->valuestring;

        //atlas_texture = make_shared<texture_t>(atlas_texture_filepath);


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
        }
        
        cJSON_Delete(root);
    }
}
}