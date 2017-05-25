#include "plant_generator.h"

#include <asdf_multiplat/utilities/utilities.h>
#include <asdf_multiplat/utilities/cjson_utils.hpp>

using namespace asdf;
using namespace asdf::util;

namespace tired_of_build_issues
{
    std::string read_text_file(std::string const& filepath) {
        //if(!is_file(filepath))
        //{
        //    //throw file_open_exception(filepath);
        //    EXPLODE("C'est une probleme");
        //}

        std::string outputstring;
        std::ifstream ifs(filepath, std::ifstream::in);
        ifs.exceptions( std::ios::failbit );

        // ASSERT(ifs.good(), "Error loading text file %s", filepath.c_str());
        if(!ifs.good())
        {
            //throw file_open_exception(filepath);
            EXPLODE("C'est une probleme");
        }

        ifs.seekg(0, std::ios::end);		//seek to the end to get the size the output string should be
        outputstring.reserve(size_t(ifs.tellg()));	//reserve necessary memory up front
        ifs.seekg(0, std::ios::beg);		//seek back to the start

        outputstring.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        ifs.close();

        return outputstring;
    }
}



namespace plantgen
{
    bool str_eq(char const* a, char const* b)
    {
        return strcmp(a, b) == 0;
    }


    std::vector<std::string> value_string_list_from_json_array(cJSON* json_array)
    {
        ASSERT(json_array, "");
        ASSERT(json_array->type == cJSON_Array, "json_array is not actually a JSON array");
        std::vector<std::string> values;

        cJSON* j = json_array->child;
        while(j)
        {
            values.emplace_back(j->valuestring);
            j = j->next;
        }

        return values;
    }

    multi_value_t multi_value_from_json(cJSON* json)
    {
        ASSERT(str_eq(json->child->string, "Multi"), "Expected a \"Multi\" json_obj");
        ASSERT(json->child->type == cJSON_Number, "JSON Value of \"Multi\" should be an integer");

        multi_value_t v;
        v.num_to_pick = json->child->valueint;
        v.values = std::move(value_string_list_from_json_array(json->child->next));   

        return v;
    }

    value_range_t value_range_from_json(cJSON* json)
    {
        ASSERT(str_eq(json->string, "Range"), "Expected a \"Range\" json_obj");

        auto str_vals = value_string_list_from_json_array(json);
        float weight = 1.0f / static_cast<float>(str_vals.size()); //just do normalized weights for now


        value_range_t v;
        for(size_t i = 0; i < str_vals.size(); ++i)
        {
            v.entries.push_back( value_range_t::entry_t{weight, std::move(str_vals[i])} );
        }

        return v;
    }

    variant_value_t value_type_from_json(cJSON* json)
    {
        if(json->type == cJSON_String)
        {
            return std::string(json->valuestring);
        }
        else if(!json->child)
        {
            EXPLODE("invalid value object %s", json->string);
            return null_value_t();
        }
        else if(str_eq(json->child->string, "Multi"))
        {
            return multi_value_from_json(json);
        }
        else if(str_eq(json->child->string, "Range"))
        {
            return value_range_from_json(json->child);
        }
        else
        {
            EXPLODE("unrecognized value object %s", json->child->string);
            return variant_value_t(null_value_t());
        }
    }


    node_t node_from_json(cJSON* json_node)
    {
        node_t node(std::string(CJSON_STR(json_node, Name)));

        cJSON* cur_child = json_node->child;
        while(cur_child)
        {
            if(str_eq(cur_child->string, "Properties"))
            {
                cJSON* property_json = cur_child->child;
                while(property_json)
                {
                    node.children.push_back(node_from_json(property_json));
                    node.children.back().parent = &node;
                    property_json = property_json->next;
                }
            }
            else if(str_eq(cur_child->string, "Values"))
            {
                cJSON* value_json = cur_child->child;
                while(value_json)
                {
                    node.values.push_back(value_type_from_json(value_json));
                    value_json = value_json->next;
                }
            }

            cur_child = cur_child->next;
        }

        return node;
    }



    ///

    node_t load_params_json(std::string const& filepath)
    {
        //std::string json_str = asdf::util::read_text_file(filepath);
        std::string json_str = tired_of_build_issues::read_text_file(filepath);
        cJSON* json_root = cJSON_Parse(json_str.c_str());

        if(!json_root)
        {
            EXPLODE( "Error parsing JSON");
            return node_t("ERROR");
        }

        node_t root = node_from_json(json_root);

        cJSON_Delete(json_root);

        return root;
    }

    /*
    A generated plant is basically a collection of random tables
    Each random table can itself contain random tables
    
    For each child
    */
    node_t generate_plant_from_json(std::string const& filepath)
    {
        node_t plant = load_params_json(filepath);
        generate_node(plant);
        return plant;
    }

}