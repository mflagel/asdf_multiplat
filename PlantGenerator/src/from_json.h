#pragma once

#include <asdf_multiplat/utilities/cjson_utils.hpp>

#include "plant_generator.h"

struct cJSON;

namespace plantgen
{
    bool str_eq(char const* a, char const* b);

    std::vector<std::string> value_string_list_from_json_array(cJSON* json_array);
    multi_value_t multi_value_from_json(cJSON* json);
    range_value_t range_value_from_json(cJSON* json);
    weighted_value_t value_type_from_json(cJSON* json);
    pregen_node_t node_from_json(cJSON* json_node);
    pregen_node_t node_from_json(stdfs::path const& filepath);
    generated_node_t generate_node_from_json(stdfs::path const& filepath);



    struct json_parse_exception : public std::runtime_error
    {
        stdfs::path json_filepath;
        const char* cjson_error_cstr;

        json_parse_exception(stdfs::path const& _json_filepath, const char* _cjson_err = nullptr)
        : std::runtime_error("JSON Parse Error: JSON file is invalid: " + _json_filepath.string()
                        + "\n--- cJSON Error ---\n" + _cjson_err)
        , json_filepath(_json_filepath)
        , cjson_error_cstr(_cjson_err)
        {}
    };

    struct json_type_exception : public std::runtime_error
    {
        stdfs::path json_filepath;
        std::string json_node_name;
        int json_type_found = -1;
        int json_type_expected = -1;

        json_type_exception(stdfs::path const& _json_filepath, cJSON* _json, int _type_expected)
        : std::runtime_error("Incorrect data type '" + std::string(cJSON_type_strings[_json->type]) 
            + "' for node '" + std::string(_json->string) + "'   found in file " + _json_filepath.string()
            + "\nExpected Type: '" + std::string(cJSON_type_strings[_type_expected]) + "'")
        , json_node_name(_json->string)
        , json_type_found(_json->type)
        , json_type_expected(_type_expected)
        {}
    };
}