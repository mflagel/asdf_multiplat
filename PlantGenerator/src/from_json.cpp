#include "from_json.h"

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


// global root_json_dir is used for resolving relative filepaths
// when including other json files as node properties / values
// I should probably devise a better method of doing this, since
// I don't think this will handle more than one level of inclusion
// if the JSON files are not all in the same directory
stdfs::path root_json_dir;


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
            ASSERT(j->type == cJSON_String, "Expected a JSON string");
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

    range_value_t range_value_from_json(cJSON* json)
    {
        ASSERT(str_eq(json->string, "Range"), "Expected a \"Range\" json_obj");
        return value_string_list_from_json_array(json);
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
            return range_value_from_json(json->child);
        }
        else
        {
            EXPLODE("unrecognized value object %s", json->child->string);
            return variant_value_t(null_value_t());
        }
    }

    pregen_node_t node_from_json(cJSON* json_node)
    {
        pregen_node_t node;
        node.name = std::string(CJSON_STR(json_node, Name));

        cJSON* cur_child = json_node->child;
        while(cur_child)
        {
            if(str_eq(cur_child->string, "Properties"))
            {
                cJSON* property_json = cur_child->child;
                while(property_json)
                {
                    node.add_child(node_from_json(property_json));
                    property_json = property_json->next;
                }
            }
            else if(str_eq(cur_child->string, "Values"))
            {
                cJSON* value_json = cur_child->child;
                while(value_json)
                {
                    // if the value is a node, load it differently
                    // since we can't return a node as part of the variant

                    //if the value is an object starting with a child named "Name", it's a node
                    if(value_json->type == cJSON_Object 
                    && value_json->child
                    && str_eq(value_json->child->string, "Name"))
                    {
                        node.value_nodes.push_back(std::move(node_from_json(value_json)));
                    }
                    else
                    {
                        node.values.push_back(std::move(value_type_from_json(value_json)));
                    }

                    value_json = value_json->next;
                }
            }
            else if(str_eq(cur_child->string, "Include"))
            {
                ASSERT(cur_child->type == cJSON_String, "Include filepath must be a string");
                stdfs::path relpath(cur_child->valuestring);

                //get path relative to this executable by using parent path
                //of the original json doc
                stdfs::path fullpath = root_json_dir.parent_path() / relpath;

                node.add_child(node_from_json(fullpath));
            }

            cur_child = cur_child->next;
        }

        return node;
    }

    pregen_node_t node_from_json(stdfs::path const& filepath)
    {
        //std::string json_str = asdf::util::read_text_file(filepath);
        std::string json_str = tired_of_build_issues::read_text_file(filepath.string());
        cJSON* json_root = cJSON_Parse(json_str.c_str());

        if(!json_root)
        {
            EXPLODE( "Error parsing JSON");
            return pregen_node_t{};
        }

        auto root = node_from_json(json_root);

        cJSON_Delete(json_root);

        return root;
    }

    generated_node_t generate_node_from_json(stdfs::path const& filepath)
    {
        // global root_json_dir is used for resolving relative filepaths
        // when including other json files as node properties / values
        root_json_dir = filepath;
        auto node = node_from_json(filepath);
        return generate_node(node);
    }

}