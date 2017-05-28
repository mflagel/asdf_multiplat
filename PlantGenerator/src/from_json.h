#pragma once

#include "plant_generator.h"

struct cJSON;

namespace plantgen
{
    bool str_eq(char const* a, char const* b);

    std::vector<std::string> value_string_list_from_json_array(cJSON* json_array);
    multi_value_t multi_value_from_json(cJSON* json);
    range_value_t range_value_from_json(cJSON* json);
    variant_value_t value_type_from_json(cJSON* json);
    pregen_node_t node_from_json(cJSON* json_node);
    pregen_node_t node_from_json(stdfs::path const& filepath);
    generated_node_t generate_node_from_json(stdfs::path const& filepath);
}