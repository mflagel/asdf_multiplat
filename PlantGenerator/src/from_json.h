#pragma once

#include "plant_generator.h"

struct cJSON;

namespace plantgen
{
    bool str_eq(char const* a, char const* b);

    std::vector<std::string> value_string_list_from_json_array(cJSON* json_array);
    multi_value_t multi_value_from_json(cJSON* json);
    value_range_t value_range_from_json(cJSON* json);
    variant_value_t value_type_from_json(cJSON* json);
    node_t node_from_json(cJSON* json_node);

    node_t load_params_json(std::string const& filepath);
    node_t generate_plant_from_json(std::string const& filepath);
}