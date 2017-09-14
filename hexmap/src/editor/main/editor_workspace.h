#pragma once

#include <experimental/filesystem>

#include <asdf_multiplat/utilities/cjson_utils.hpp>

namespace asdf {
namespace hexmap {
namespace editor
{
    
    struct editor_settings_t
    {

    };

    struct editor_workspace_t
    {
        std::vector<std::experimental::filesystem::path> recently_opened;

        editor_settings_t settings;

        void update_recently_opened_with(std::experimental::filesystem::path const&);
        void remove_non_existing_recents();

        cJSON* to_JSON() const;
        void from_JSON(cJSON*);
    };

}
}
}