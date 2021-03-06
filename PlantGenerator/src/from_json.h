#pragma once

#include <asdfm/utilities/cjson_utils.hpp>

#include "plant_generator.h"

struct cJSON;

namespace plantgen
{
    std::vector<std::string> value_string_list_from_json_array(cJSON* json_array);
    multi_value_t multi_value_from_json(cJSON* json);
    range_value_t range_value_from_json(cJSON* json);
    weighted_value_t value_type_from_json(cJSON* json);
    user_value_t user_value_from_json(cJSON const& json);
    user_data_node_t user_node_from_json(cJSON const& json);
    pregen_node_t node_from_json(cJSON* json_node);
    pregen_node_t node_from_json(stdfs::path const& filepath);
    std::vector<pregen_node_t> nodes_from_json(cJSON* json_array);
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
        //int json_type_expected = -1;
        std::vector<int> json_types_expected;

        json_type_exception(stdfs::path const& _json_filepath, cJSON const& _json, std::vector<int> _types_expected)
        : std::runtime_error("")
        , json_node_name("")
        , json_type_found(_json.type)
        , json_types_expected(std::move(_types_expected))
        {
            if(json_type_found < 0 || json_type_found >= cJSON_type_strings.size())
                json_type_found = cJSON_type_strings.size() - 1;
                
            if(_json.string)
                json_node_name = std::string(_json.string);
            else
                json_node_name = "Unnamed Node";

            std::string str = "Incorrect data type '" + std::string(cJSON_type_strings[json_type_found])
                + "' for node '" + json_node_name;

            if(!_json_filepath.empty())
            {
                str += "'   found in file " + _json_filepath.string();
            }

            if(json_types_expected.size() == 1)
            {
                str.append("\nExpected Type: '" + std::string(cJSON_type_strings[json_types_expected[0]]) + "'");
            }
            else if(json_types_expected.size() > 1)
            {
                str.append("\nExpected Types:");

                for(auto type : json_types_expected)
                {
                    if(type < 0 || type >= cJSON_type_strings.size())
                        type = cJSON_type_strings.size() - 1;

                    str.append("\n    '" + std::string(cJSON_type_strings[type])  + "'");
                }
            }

            static_cast<std::runtime_error&>(*this) = std::runtime_error(str);
        }

        json_type_exception(stdfs::path const& _json_filepath, cJSON const& _json, int _type_expected)
        : json_type_exception(_json_filepath, _json, std::vector<int>{_type_expected})
        {}

        json_type_exception(stdfs::path const& _json_filepath, cJSON const& _json, std::initializer_list<int> _types_expected)
        : json_type_exception(_json_filepath, _json, std::vector<int>{_types_expected})
        {}
    };

    struct invalid_weight_exception : std::runtime_error
    {
        invalid_weight_exception(std::string weight_string)
        : std::runtime_error("Invalid Weight Specifier for \"" + weight_string + "\"")
        {
        }
    };
}