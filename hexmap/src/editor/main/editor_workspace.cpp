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

        bool exists = it != recently_opened.end();
        bool exists_at_end = (recently_opened.end() - it) == 1;

        /// if the file exists but isnt at the end, pull it out and re-add
        if(exists && !exists_at_end)
            recently_opened.erase(it);

        if(!exists || !exists_at_end)
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
