#include "plant_generator.h"

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <type_traits>

#include <asdf_multiplat/main/asdf_defs.h>

#include "from_json.h"

namespace plantgen
{
    //std::vector<std::string> roll_multi_value(multi_value_t const& m);


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



    std::vector<std::string> roll_multi_value(multi_value_t const& m)
    {
        if(m.num_to_pick >= m.values.size())
            return m.values;

        std::vector<size_t> inds(m.values.size());
        std::iota(inds.begin(), inds.end(), size_t(0)); //0, 1, 2, 3, ..., size-1
        std::shuffle(inds.begin(), inds.end(), std::mt19937{std::random_device{}()});

        std::vector<std::string> output;
        output.reserve(m.num_to_pick);
        
        for(size_t i = 0; i < m.num_to_pick; ++i)
            output.push_back(m.values[inds[i]]);

        return output;
    }

    /// Runtime version (compile-time visitor pattern doesn't compile in msvc)
    std::vector<std::string> roll_value(variant_value_t const& variant_val)
    {
        std::vector<std::string> output;

        if(auto* s = std::get_if<std::string>(&variant_val))
        {
            output.push_back(*s);
        }
        else if(auto* r = std::get_if<value_range_t>(&variant_val))
        {
            for(auto& e : r->entries)
            {
                char buf[100];
                snprintf(buf, 100, "%0.2f", e.weight);

                output.emplace_back(std::string(buf) + " " + e.name);
            }
        }
        else if(auto* m = std::get_if<multi_value_t>(&variant_val))
        {
            auto rolled_multi = roll_multi_value(*m);
            output.insert(output.end(), rolled_multi.begin(), rolled_multi.end());
        }
        else
        {
            EXPLODE("unexpected variant sub-type");
        }

        return output;
    }

    std::vector<std::string> roll_values(value_list_t const& variant_values)
    {
        auto rand_ind = random_int(variant_values.size() - 1);
        return roll_value(variant_values[rand_ind]);
    }


    void generate_node(node_t& node)
    {
        for(auto& child : node.children)
            generate_node(child);

        if(node.values.empty())
        {
            return;
        }

        
        node.generated_values = roll_values(node.values);
    }


    node_t generate_plant_from_file(std::string const& filepath)
    {
        using namespace std;

        auto dot_marker = filepath.find_last_of('.');
        auto ext = filepath.substr(dot_marker+1);
        if(ext == "json" || ext == "jsn")
        {
            return generate_plant_from_json(filepath);
        }
        else if(ext == "yaml" || ext == "yml")
        {
            cout << "TODO: yaml support";
        }
        else
        {
            cout << "Filetype not recognized";
        }
    }



    constexpr size_t indent_amt = 2;

    void print_node(node_t const& node, size_t level)
    {
        using namespace std;

        string indent(" ", level * indent_amt);
        cout << indent << node.name << "\n";
        
        for(auto const& v : node.generated_values)
        {
            cout << indent << "  " << v << "\n";
        }

        if(node.children.size() > 0)
        {
            for(auto& child : node.children)
                print_node(child, level + 1);
        }
    }

    void print_plant(node_t& plant)
    {
        print_node(plant, 0);
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

    std::string filepath;

    if(argc > 1)
        filepath = std::string(argv[1]);
    else
        filepath = std::string("../data/mushroom.json");


    for(size_t i = 0; i < 100; ++i)
    {
        print_plant(generate_plant_from_file(filepath));
    }

    return 0;
}