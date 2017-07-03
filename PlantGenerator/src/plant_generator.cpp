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

using namespace std;

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


    template <typename L>
    uint32_t total_weight(L const& list)
    {
        uint32_t total = 0;

        for(auto const& v : list)
            total += v.weight;

        return total;
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
        for(size_t i = 0; i < r.size() - 1; ++i)
        {
            int roll = 0;

            if(counter < 100)
                roll = random_int(100 - counter);

            counter += roll;
            output.push_back(std::to_string(roll) + "% " + r[i]);
        }

        //push remaining
        output.push_back(std::to_string(100 - counter) + "% " + r.back());

        return output;
    }


    /// Runtime version (compile-time visitor pattern doesn't compile in msvc)
    std::vector<std::string> roll_value(weighted_value_t const& variant_val)
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

    generated_node_t roll_values(value_list_t const& values, std::vector<pregen_node_t> const& value_nodes)
    {
        if(values.empty() && value_nodes.empty())
            return generated_node_t();

        uint32_t total = total_weight(values) + total_weight(value_nodes);

        int roll = random_int(total);

        for(size_t i = 0; i < values.size(); ++i)
        {
            if(roll <= values[i].weight)
            {
                generated_node_t node;
                node.generated_values = std::move(roll_value(values[i]));
                return node;
            }

            roll -= values[i].weight;
        }

        //if no value from the value list has been hit yet, continue onward into value_nodes
        for(size_t i = 0; i < value_nodes.size(); ++i)
        {
            if(roll <= value_nodes[i].weight)
            {
                generated_node_t node(value_nodes[i].name);
                //node.add_child(generate_node(value_nodes[i]));
                auto gend_node = generate_node(value_nodes[i]);
                node.add_value_node(std::move(gend_node));
                return node;
            }

            roll -= value_nodes[i].weight;
        }

        EXPLODE("Random roll was too large, ran out of values and value_nodes");
        return generated_node_t("ERROR");
    }


    generated_node_t roll_values(pregen_node_t const& node)
    {
        return roll_values(node.values, node.value_nodes);
    }

    pregen_node_t node_from_file(stdfs::path const& filepath)
    {
        using namespace std;

        auto ext = filepath.extension();
        if(ext == ".json")
        {
            return node_from_json(filepath);
        }
        else if(ext == ".yaml" || ext == ".yml")
        {
            cout << "TODO: yaml support";
            return pregen_node_t();
        }

        //TODO: exception?
        if(stdfs::is_directory(filepath))
            cout << filepath.string() << " is a directory, not a file";
        else
            cout << "Filetype " << ext << " not recognized";

        return pregen_node_t();
    }

    generated_node_t generate_node(pregen_node_t const& pre_node)
    {
        generated_node_t node;
        node.name = pre_node.name;

        for(auto const& child : pre_node.children)
        {
            node.add_child(generate_node(child));
        }

        if(pre_node.values.size() > 0 || pre_node.value_nodes.size() > 0)
        {
            generated_node_t rolled = roll_values(pre_node);
            node.merge_with(rolled);
        }

        auto search = pre_node.user_data.find("PrintString");
        if(search != pre_node.user_data.end())
        {
            node.print_string = std::get<std::string>(search->second);
        }

        return node;
    }

    generated_node_t generate_node_from_file(stdfs::path const& filepath)
    {
        auto node = node_from_file(filepath);
        return generate_node(node);
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
        for(size_t i = 0; i < indent_level; ++i)
            indent_str.insert(indent_str.end(), indent_cstr, indent_cstr + 4);

        return indent_str;
    }


    /// TODO: factor out similarities with below?
    string to_string(pregen_node_t const& node, size_t const depth, size_t level)
    {
        if(level > depth)
            return "";

        stringstream s;

        auto indent = indenation_string(level);
        s << indent << node.name_string() << "\n";

        if(level + 1 > depth)
            return s.str();

        for(auto const& child : node.children)
            s << to_string(child, depth, level + 1);

        indent.append(indenation_string(1));

        for(auto const& value : node.values)
            s << indent << value << "\n";

        for(auto const& vnode : node.value_nodes)
            s << to_string(vnode, depth, level + 1);

        for(auto const& user_vals : node.user_data)
            s << indent << user_vals.first << ": " << user_vals.second << "\n";

        return s.str();
    }

    string to_string(generated_node_t const& node, size_t depth, size_t level)
    {
        if(level > depth)
            return "";

        stringstream s;

        auto indent = indenation_string(level);
        s << indent << node.name_string() << "\n";

        if(level + 1 > depth)
            return s.str();
        
        for(auto const& child : node.children)
            s << to_string(child, depth, level + 1);

        indent.append(indenation_string(1));

        for(auto const& value : node.generated_values)
            s << indent << value << "\n";

        for(auto const& vn : node.value_nodes)
            s << to_string(vn, depth, level + 1);

        return s.str();
    }

    void print_node(pregen_node_t const& node, size_t depth, size_t level)
    {
        cout << to_string(node, depth, level);
    }

    void print_node(generated_node_t const& node, size_t depth, size_t level)
    {
        cout << to_string(node, depth, level);
    }
}