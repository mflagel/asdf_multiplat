#include "editor_workspace.h"

#include <algorithm>

using namespace std;
namespace stdfs = std::experimental::filesystem;

namespace asdf {
    using namespace util;

namespace hexmap {
namespace editor
{
    void editor_workspace_t::update_recently_opened_with(std::experimental::filesystem::path const& path)
    {
        auto it = std::find(recently_opened.begin(), recently_opened.end(), path);

        /// if path is already in recents and it's not at the end
        /// erase it from recents (it'll get re-added)
        if((recently_opened.end() - it) > 1)
        {
            recently_opened.erase(it);
        }

        recently_opened.push_back(path);
    }

    cJSON* editor_workspace_t::to_JSON() const
    {
        CJSON_CREATE_ROOT();

        cJSON* str_arr = cJSON_CreatePathArray(recently_opened);
        cJSON_AddItemToObject(root, "recently_opened", str_arr);

        return root;
    }

    void editor_workspace_t::from_JSON(cJSON* root)
    {
        auto path_from_json = [this](cJSON* j)
        {
            ASSERT(j && j->type == cJSON_String,"");
            recently_opened.emplace_back(j->valuestring);
        };

        CJSON_FOR_EACH_ITEM_VECTOR(recently_opened, path_from_json);
    }
}
}
}
