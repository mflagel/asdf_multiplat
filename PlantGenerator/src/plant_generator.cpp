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

    generated_node_t roll_values(value_list_t const& variant_values, std::vector<pregen_node_t> const& value_nodes)
    {
        size_t max_variant_inds = variant_values.size() - int(!variant_values.empty());
        size_t max_node_inds = value_nodes.size() - int(!value_nodes.empty());


        size_t num_rollables = variant_values.size() + value_nodes.size();

        if(num_rollables == 0)
            return generated_node_t();


        generated_node_t rolled_node;
        auto rand_ind = random_int(num_rollables - 1);


        /// FIXME this it not a particularily elegant method of
        /// figuring out which list to index into
        if(rand_ind > max_variant_inds || variant_values.empty())
        {
            auto const& value_node = value_nodes[rand_ind - variant_values.size()];

            rolled_node.name = value_node.name;
            rolled_node.add_child(generate_node(value_node));
        }
        else
        {
            rolled_node.generated_values = roll_value(variant_values[rand_ind]);
        }

        return rolled_node;
    }

    generated_node_t roll_values(pregen_node_t const& node)
    {
        return roll_values(node.values, node.value_nodes);
    }


    generated_node_t generate_node(pregen_node_t const& pre_node)
    {
        generated_node_t node;
        node.name = pre_node.name;

        for(auto const& child : pre_node.children)
        {
            node.add_child(generate_node(child));
        }

        generated_node_t rolled = roll_values(pre_node);
        node.children.insert(node.children.end(), rolled.children.begin(), rolled.children.end());
        node.generated_values = std::move(rolled.generated_values);

        return node;
    }


    generated_node_t generate_node_from_file(stdfs::path const& filepath)
    {
        using namespace std;

        auto ext = filepath.extension();
        if(ext == ".json")
        {
            return generate_node_from_json(filepath);
        }
        else if(ext == ".yaml" || ext == ".yml")
        {
            cout << "TODO: yaml support";
            return generated_node_t();
        }

        cout << "Filetype " << ext << " not recognized";
        return generated_node_t();
    }


    constexpr size_t indent_amt = 4;
    // constexpr char indent_char = ' ';
    // constexpr char indent_tree_marker = ':';
    constexpr char const* indent_cstr = ":   ";

    std::string indenation_string(size_t indent_level)
    {
        if(indent_level == 0)
            return "";

        // std::string indent_str(indent_level * indent_amt, indent_char);
        // indent_str[indent_str.size()-indent_amt] = indent_tree_marker;

        std::string indent_str;
        for(size_t i = 1; i < indent_level; ++i)
            indent_str.insert(indent_str.end(), indent_cstr, indent_cstr + 4);

        return indent_str;
    }

    void print_node(generated_node_t const& node, size_t level)
    {
        using namespace std;

        auto indent = indenation_string(level);

        cout << indent << node.name << "\n";
        
        if(node.children.size() > 0)
        {
            for(auto const& child : node.children)
                print_node(child, level + 1);
        }

        for(auto const& value : node.generated_values)
        {
            indent = indenation_string(level+1);
            cout << indent << value << "\n";
        }
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

    try{
        generated_node_t plant = generate_node_from_file(filepath);
        print_node(plant);
    }
    catch(std::runtime_error const& e)
    {
        cout << e.what() << "\n";
    }

    return 0;
}