#include "stdafx.h"
#include "terrain_bank.h"

using namespace std;

namespace asdf
{
    using namespace util;
    using namespace data;

namespace hexmap {
namespace data
{
    void terrain_bank_t::load_from_file(std::string const& filepath)
    {
        std::string json_str = read_text_file(filepath);
        cJSON* root = cJSON_Parse(json_str.c_str());
        ASSERT(root, "Error loading imported textures json file");

        vector<entry_t> terrain;
        CJSON_GET_ITEM_VECTOR(terrain);

        std::vector<const char*> filepaths;
        for(auto const& t : terrain)
            filepaths.push_back(t.asset.c_str());

        add_textures(filepaths);

        cJSON_Delete(root);
    }

    void terrain_bank_t::entry_t::from_JSON(cJSON* root)
    {
        CJSON_GET_STR(name);
        CJSON_GET_STR(asset);
    }
}
}
}