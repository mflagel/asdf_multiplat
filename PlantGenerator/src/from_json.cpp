#include "from_json.h"

#include <unordered_map>

//#include <asdfm/utilities/utilities.h>

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


// custom hash function for std::path so that I can use it as
// a key in unordered_map
namespace std
{
template<>
struct hash<stdfs::path> {
    size_t operator()(const stdfs::path &p) const {
        return std::hash<std::string>()(canonical(p).string());
    }
};
}



namespace plantgen
{
    // global stack used for resolving relative filepaths
    // when including other json files as node properties / values
    // I should probably devise a better method of doing this, since
    // I don't think this will handle more than one level of inclusion
    // if the JSON files are not all in the same directory

    //FIXME make this not global
    std::vector<stdfs::path> node_path_stack;
    std::unordered_map<stdfs::path, pregen_node_t> include_cache;

    stdfs::path current_node_path()
    {
        if(node_path_stack.size() > 0)
            return node_path_stack.back();

        return stdfs::path();
    }
    ///


    std::vector<std::string> value_string_list_from_json_array(cJSON* json_array)
    {
        ASSERT(json_array, "");
        ASSERT(json_array->type == cJSON_Array, "json_array is not actually a JSON array");
        std::vector<std::string> values;

        cJSON* j = json_array->child;
        while(j)
        {
            if(j->type != cJSON_String)
            {
                throw json_type_exception(current_node_path(), *j, cJSON_String);
            }

            values.emplace_back(j->valuestring);
            j = j->next;
        }

        return values;
    }


    /// WIP trying to support non-string multi values
    multi_value_t multi_value_from_json(cJSON* json)
    {
        ASSERT(str_eq(json->child->string, "Multi"), "Expected a \"Multi\" json_obj");
        ASSERT(json->child->type == cJSON_Number, "JSON Value of \"Multi\" should be an integer");

        multi_value_t v;
        v.num_to_pick = json->child->valueint;

        cJSON* value_array_json = json->child->next;
        for(cJSON* j = value_array_json->child; j; j = j->next)
        {
            auto n = node_from_json(j);
            auto pn = std::make_unique<pregen_node_t>(n);
            v.values.push_back(std::move(pn));
        }

        return v;
    }

    range_value_t range_value_from_json(cJSON* json)
    {
        ASSERT(str_eq(json->string, "Range"), "Expected a \"Range\" json_obj");
        return value_string_list_from_json_array(json);
    }

    weighted_value_t value_type_from_json(cJSON* json)
    {
        if(json->type == cJSON_String)
        {
            return value_from_string(std::string(json->valuestring));
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
            return weighted_value_t(null_value_t());
        }
    }

    user_value_t user_value_from_json(cJSON const& json)
    {
        switch(json.type)
        {
            case cJSON_True:
                return user_value_t(bool(true));

            case cJSON_False:
                return user_value_t(bool(false));

            case cJSON_Number:
                return user_value_t(json.valuedouble);

            case cJSON_String:
                return user_value_t(std::string(json.valuestring));

            default:
                throw json_type_exception(current_node_path(), json,
                    {cJSON_True, cJSON_False, cJSON_Number, cJSON_String});
                break;
        };

        return nullptr;
    }

    user_data_node_t user_node_from_json(cJSON const& json)
    {
        user_data_node_t node;

        if(json.string)
            node.name = json.string;

        switch(json.type)
        {
            case cJSON_Object:
            case cJSON_Array:
            {
                cJSON* cur_child = json.child;
                while(cur_child)
                {
                    node.add_child(user_node_from_json(*cur_child));

                    cur_child = cur_child->next;
                }

                break;
            }

            default:
                node.value = user_value_from_json(json);
                break;
        }

        return node;
    }

    void include_to_node(pregen_node_t& node, stdfs::path const& relpath)
    {
        if(relpath.empty())
        {
            throw std::runtime_error("Include path is empty for \"" + node.name + "\"");
        }

        
        stdfs::path parent_path;
        if(node_path_stack.size() > 0)
        {
            ASSERT(current_node_path() == stdfs::canonical(current_node_path()), "current_node_path is not canonical");
            parent_path = current_node_path().parent_path();
        }
        else
        {
            /// if loading json from raw string instead of file, current_node_path will be empty
            /// in which case assume paths are relative to current working dir
            parent_path = ".";
        }


        stdfs::path fullpath = parent_path / relpath;
        stdfs::path canonical_path;
        try
        {
            canonical_path = stdfs::canonical(fullpath);
        }
        catch (stdfs::filesystem_error const& fs_e)
        {
            if(fs_e.code() == std::errc::no_such_file_or_directory)
            {
                throw include_not_found_exception{current_node_path(), canonical_path};
            }
            else
            {
                throw fs_e;
            }
        }


        /// If this path is already in the include stack
        /// it means there is a cycle in the include graph
        for(auto const& inc : node_path_stack)
        {
            if(canonical_path == inc)
            {
                throw include_cycle_exception(node_path_stack);
            }
        }


        
        pregen_node_t included_node;

        
        auto cached_node_entry = include_cache.find(canonical_path);
        if(cached_node_entry != include_cache.end())
        {
            included_node = cached_node_entry->second;
        }
        else
        {
            try
            {
                included_node = node_from_file(canonical_path);
            }
            catch(stdfs::filesystem_error const& fs_e)
            {
                if(fs_e.code() == std::errc::no_such_file_or_directory)
                {
                    throw include_not_found_exception{current_node_path(), canonical_path};
                }
                else
                {
                    throw fs_e;
                }
            }
        }


        include_cache.insert({canonical_path, included_node});


        /// Merge node and fiddle with weights and names
        auto prev_weight = node.weight;
        node.merge_with(std::move(included_node));
        node.weight = prev_weight;

        /// TODO: check for existance of name value?
        ///       as opposed to just checking for an empty name
        if(node.name == "")
        {
            node.name = included_node.name;
            node.sub_name = "";
        }
    }

    pregen_node_t object_node_from_json(cJSON* json_node)
    {
        ASSERT(json_node->type == cJSON_Object, "Expected cJSON type to be Object");

        pregen_node_t node;

        cJSON* cur_child = json_node->child;
        while(cur_child)
        {
            if(str_eq(cur_child->string, "Name"))
            {
                node.name = cur_child->valuestring;
            }
            else if(str_eq(cur_child->string, "Properties"))
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

                    // is range or multi
                    bool is_value_type_node = value_json->type == cJSON_Object 
                                           && value_json->child
                                           &&  (str_eq(value_json->child->string, "Range")
                                             || str_eq(value_json->child->string, "Multi"))
                                           ;

                    if(is_value_type_node || value_json->type != cJSON_Object)
                    {
                        node.value_nodes.emplace_back(value_type_from_json(value_json));
                    }
                    else
                    {
                        node.value_nodes.push_back(std::move(node_from_json(value_json)));
                    }

                    value_json = value_json->next;
                }
            }
            else if(str_eq(cur_child->string, "Include"))
            {
                if(cur_child->type != cJSON_String)
                    throw json_type_exception(current_node_path(), *cur_child, cJSON_String);
                    
                include_to_node(node, stdfs::path(cur_child->valuestring));
            }
            else if(str_eq(cur_child->string, "Merge"))
            {
                ASSERT(cur_child, "");
                if(cur_child->type != cJSON_Array)
                    throw json_type_exception(current_node_path(), *cur_child, cJSON_Array);

                std::vector<pregen_node_t> merge_nodes;

                cJSON* arr_child = cur_child->child;
                while(arr_child)
                {
                    switch(arr_child->type)
                    {
                        case cJSON_String:
                        {
                            pregen_node_t inc_node;
                            include_to_node(inc_node, stdfs::path(arr_child->valuestring));
                            merge_nodes.push_back(std::move(inc_node));
                            break;
                        }

                        case cJSON_Object:
                            merge_nodes.push_back(node_from_json(arr_child));
                            break;

                        default:
                            throw json_type_exception(current_node_path(), *arr_child,
                                                      {cJSON_String, cJSON_Object});
                            break;
                    }
                    
                    arr_child = arr_child->next;
                }

                auto merged_node = node_from_merged_nodes(merge_nodes);
                node.merge_with(merged_node);

                if(node.name.empty())
                {
                    node.name = merged_node.name;
                    node.sub_name.clear();
                }
            }
            else if(str_eq(cur_child->string, "Weight"))
            {
                switch(cur_child->type)
                {
                case cJSON_Number:
                    node.weight = cur_child->valueint;
                    break;

                case cJSON_String:
                {
                    int weight = weight_from_string(std::string(cur_child->valuestring));
                    if(weight >= 0 || weight == weight_inherit_code)
                    {
                        node.weight = weight;
                    }
                    else
                    {
                        throw invalid_weight_exception(node.name);
                    }

                    break;
                }

                default:
                    throw json_type_exception(current_node_path(), *cur_child,
                        {cJSON_Number, cJSON_String});
                }
            }
            else
            {
                node.user_data.add_child(user_node_from_json(*cur_child));
            }

            cur_child = cur_child->next;
        }

        //handle weight inhereit
        if(node.weight == weight_inherit_code)
        {
            node.weight = total_weight(node.value_nodes);
        }

        return node;
    }

    std::vector<pregen_node_t> nodes_from_json(cJSON* json_array)
    {
        ASSERT(json_array, "");
        ASSERT(json_array->type == cJSON_Array, "Expected cJSON type to be Array");

        std::vector<pregen_node_t> nodes;

        cJSON* cur_child = json_array->child;
        while(cur_child)
        {
            nodes.push_back(node_from_json(cur_child));
            cur_child = cur_child->next;
        }

        return nodes;
    }

    pregen_node_t array_node_from_json(cJSON* json_array)
    {
        pregen_node_t node;
        node.children = nodes_from_json(json_array);
        return node;
    }

    pregen_node_t node_from_json(cJSON* json_node)
    {
        ASSERT(json_node, "");

        switch(json_node->type)
        {
            case cJSON_Object:
                return object_node_from_json(json_node);

            case cJSON_Array:
                return array_node_from_json(json_node);

            default:
            {
                pregen_node_t node;
                node.value = value_type_from_json(json_node);
                return node;
            }
        }
    }

    pregen_node_t node_from_json(stdfs::path const& _filepath)
    {
        stdfs::path canonical_path;

        try
        {
            canonical_path = stdfs::canonical(_filepath);
        }
        catch (stdfs::filesystem_error const& fs_e)
        {
            throw fs_e;
        }

        if(stdfs::is_directory(canonical_path))
            throw stdfs::filesystem_error("Cannot create node from JSON", canonical_path, std::make_error_code(std::errc::is_a_directory));

        if(!stdfs::is_regular_file(canonical_path))
            throw stdfs::filesystem_error("Cannot create node from JSON", canonical_path, std::make_error_code(std::errc::not_supported));

        std::string json_str = tired_of_build_issues::read_text_file(canonical_path.string());
        cJSON* json_root = cJSON_Parse(json_str.c_str());

        if(!json_root)
        {
            throw json_parse_exception(canonical_path, cJSON_GetErrorPtr());
        }
        
        node_path_stack.push_back(canonical_path);

        auto node = node_from_json(json_root);
        cJSON_Delete(json_root);

        node_path_stack.pop_back();

        return node;
    }

    generated_node_t generate_node_from_json(stdfs::path const& filepath)
    {
        auto node = node_from_json(filepath);
        return generate_node(node);
    }

}