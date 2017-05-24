#include "plant_generator.h"

#include <array>
#include <chrono>
#include <functional>
#include <random>

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
    std::mt19937 mt_rand( std::chrono::high_resolution_clock::now().time_since_epoch().count() );

    int random_int(size_t min, size_t max)
    {
        std::uniform_int_distribution<int> dis(min,max);
        return dis(mt_rand);
    }

    int random_int(size_t max)
    {
        return random_int(0, max);
    }


    node_t::node_t(std::string _name)
    : name(std::move(_name))
    {
    }

    bool str_eq(char const* a, char const* b)
    {
        return strcmp(a, b) == 0;
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
                    node.values.emplace_back(std::string(value_json->valuestring));
                    value_json = value_json->next;
                }
            }

            cur_child = cur_child->next;
        }

        return node;
    }


    node_t load_params(std::string const& filepath)
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

    
    //collapses the value list into one randomly chosen value
    //uses uniform distribution
    void roll_values(node_t& node)
    {
        for(auto& child : node.children)
            roll_values(child);

        if(node.values.size() > 1)
        {
            auto rand_ind = random_int(node.values.size() - 1);
            node.values[0] = node.values[rand_ind];
            node.values.resize(1);
        }
    }


    /*
    A generated plant is basically a collection of random tables
    Each random table can itself contain random tables
    
    For each child
    */
    node_t generate_plant(std::string const& filepath)
    {
        node_t plant = load_params(filepath);
        roll_values(plant);
        return plant;
    }

    constexpr size_t indent_amt = 2;

    void print_node(node_t const& node, size_t level)
    {
        using namespace std;

        string indent(" ", level * indent_amt);
        cout << indent << node.name << endl;
        
        if(node.values.size() > 0)
        {
            cout << indent << "  " << node.values[0] << endl;
        }

        if(node.children.size() > 0)
        {
            for(auto& child : node.children)
                print_node(child, level + 1);
        }
    }

    void print_tree(node_t& tree)
    {
        print_node(tree, 0);
    }
}


using namespace plantgen;

// undef main because apparently SDL redefines 'main' as SDL_main
// which is super obnoxious
#undef main

int main(int argc, char* argv[])
{
    using namespace std;
    if(argc == 0)
    {
        cout << "asdf";
        return 0;
    }

    auto plant = generate_plant(argv[1]);
    print_tree(plant);
    std::cout << std::endl;

    return 0;
}