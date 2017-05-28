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

    int random_int(uint32_t min, uint32_t max)
    {
        std::uniform_int_distribution<int> dis(min,max);
        return dis(mt_rand);
    }

    int random_int(uint32_t max)
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

    std::vector<std::string> roll_range_value(range_value_t const& r)
    {
        std::vector<std::string> output;
        output.reserve(r.size());

        uint32_t counter = 0;
        for(size_t i = 0; i < r.size(); ++i)
        {
            int roll = 0;

            if(counter < 100)
                roll = random_int(100 - counter);

            counter += roll;
            output.push_back(std::to_string(roll) + "% " + r[i]);
        }

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
        else if(auto* r = std::get_if<range_value_t>(&variant_val))
        {
            auto rolled_range = roll_range_value(*r);
            output.insert(output.end(), rolled_range.begin(), rolled_range.end());
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

    std::vector<std::string> roll_values(value_list_t const& variant_values, std::vector<node_t> const& value_nodes)
    {
        size_t max_variant_inds = variant_values.size() - int(!variant_values.empty());
        size_t max_node_inds = value_nodes.size() - int(!value_nodes.empty());


        size_t num_rollables = variant_values.size() + value_nodes.size();

        if(num_rollables == 0)
            return std::vector<std::string>();


        auto rand_ind = random_int(num_rollables - 1);


        /// FIXME this it not a particularily elegant method of
        /// figuring out which list to index into
        if(rand_ind > max_variant_inds || variant_values.empty())
        {
            node_t const& node = value_nodes[rand_ind - variant_values.size()];

            ///TODO: handle node children

            return roll_values(node);
        }
        else
        {
            return roll_value(variant_values[rand_ind]);
        }
    }

    std::vector<std::string> roll_values(node_t const& node)
    {
        return roll_values(node.values, node.value_nodes);
    }


    void generate_node(node_t& node)
    {
        for(auto& child : node.children)
            generate_node(child);

        if(node.values.empty() && node.value_nodes.empty())
        {
            return;
        }

        
        node.generated_values = roll_values(node);
    }


    node_t generate_plant_from_file(stdfs::path const& filepath)
    {
        using namespace std;

        auto ext = filepath.extension();
        if(ext == ".json")
        {
            return generate_plant_from_json(filepath);
        }
        else if(ext == ".yaml" || ext == ".yml")
        {
            cout << "TODO: yaml support";
            return node_t();
        }

        cout << "Filetype not recognized";
        return node_t();
    }



    constexpr size_t indent_amt = 4;

    void print_node(node_t const& node, size_t level)
    {
        using namespace std;

        string indent(" ",0, level * indent_amt);
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
        filepath = std::string("../data/small_plant.json");


    node_t plant = generate_plant_from_file(filepath);
    print_plant(plant);

    return 0;
}