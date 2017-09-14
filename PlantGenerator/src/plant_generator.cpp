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
    std::mt19937 mt_rand( std::chrono::high_resolution_clock::now().time_since_epoch().count() );

    int32_t random_int(uint32_t min, uint32_t max)
    {
        std::uniform_int_distribution<int> dis(min,max);
        return dis(mt_rand);
    }

    int32_t random_int(uint32_t max)
    {
        return random_int(0, max);
    }


    /// non-format-specific parsing
    int weight_from_string(std::string const& weight_str)
    {
        if(weight_str[0] == '%')
        {
            if(weight_str.size() == 1)
            {
                return weight_inherit_code;
            }

            auto space_pos = weight_str.find_first_of(' ');

            if(space_pos > 1)
            {
                try
                {
                    return std::stoi(weight_str.substr(1, space_pos));
                }
                catch (...)
                {
                    throw invalid_weight_exception(weight_str);
                }
                
            }
            else
            {
                throw invalid_weight_exception(weight_str);
            }
        }

        return -1;
    }

    weighted_value_t value_from_string(std::string const& value_string)
    {
        weighted_value_t v = value_string;

        
        if(value_string[0] == '%')
        {
            auto space_pos = value_string.find_first_of(' ');

            if(space_pos > 1)
            {
                v = value_string.substr(space_pos+1, std::string::npos);
                v.weight = weight_from_string(value_string);
            }
            else
            {
                throw invalid_weight_exception(value_string);
            }
        }

        return v;
    }



    template <typename L>
    uint32_t total_weight(L const& list)
    {
        uint32_t total = 0;

        for(auto const& v : list)
            total += v.weight;

        return total;
    }

    std::vector<pregen_node_t> roll_multi_value(multi_value_t const& m)
    {
        // if(m.num_to_pick >= m.values.size())
        //     return m.values;

        std::vector<size_t> inds(m.values.size());
        std::iota(inds.begin(), inds.end(), size_t(0)); //0, 1, 2, 3, ..., size-1
        std::shuffle(inds.begin(), inds.end(), std::mt19937{std::random_device{}()});

        std::vector<pregen_node_t> output;
        output.reserve(m.num_to_pick);
        
        for(size_t i = 0; i < m.num_to_pick; ++i)
            output.push_back(*m.values[inds[i]]);

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
            // output.insert(output.end(), rolled_multi.begin(), rolled_multi.end());
        }
        else
        {
            EXPLODE("unexpected variant sub-type");
        }

        return output;
    }

    generated_node_t roll_values(pregen_node_t const& node)
    {
        generated_node_t output_node;

        auto const& value_nodes = node.value_nodes;
        output_node.num_rollable_values = value_nodes.size();

        WARN_IF(value_nodes.empty(), "Rolling values for an empty list");
        if(value_nodes.empty())
            return generated_node_t();

        int roll = random_int(total_weight(value_nodes));


        //if no value from the value list has been hit yet, continue onward into value_nodes
        for(size_t i = 0; i < value_nodes.size(); ++i)
        {
            if(roll <= value_nodes[i].weight)
            {
                output_node.name = value_nodes[i].name;
                output_node.value_index = i;

                if(value_nodes[i].value.index() > 0)
                {
                    output_node.generated_values = roll_value(value_nodes[i].value);
                }
                else
                {
                    auto gend_node = generate_node(value_nodes[i]);
                    output_node.add_value_node(std::move(gend_node));
                }
                
                return output_node;
            }

            roll -= value_nodes[i].weight;
        }

        EXPLODE("Random roll was too large, ran out of values and value_nodes");
        return generated_node_t("ERROR");
    }

    pregen_node_t node_from_merged_nodes(std::vector<pregen_node_t> const& nodes)
    {
        if(nodes.empty())
            return pregen_node_t();

        pregen_node_t node = nodes[0];

        for(size_t i = 1; i < nodes.size(); ++i)
        {
            node.merge_with(nodes[i]);
        }

        return node;
    }

    pregen_node_t node_from_file(stdfs::path const& filepath)
    {
        using namespace std;

        std::error_code ec;
        bool exists = stdfs::exists(filepath, ec);
        if(!ec && !exists) //if no error code from stdfs::exists and file does not exists, set ec
            ec = std::make_error_code(std::errc::no_such_file_or_directory);

        if(ec)
        {
            throw stdfs::filesystem_error("Cannot create node from file", filepath, ec);
        }

        auto ext = filepath.extension();
        if(ext == ".json")
        {
            return node_from_json(filepath);
        }
        else if(ext == ".yaml" || ext == ".yml")
        {
            throw invalid_file_exception{filepath, std::string("TODO: yaml support")};
        }


        if(stdfs::is_directory(filepath))
            throw invalid_file_exception{filepath, std::string("file is actually a directory")};
        else
            throw invalid_file_exception{filepath, stdfs::path("json")};

        return pregen_node_t();
    }

    generated_node_t generate_node(pregen_node_t const& pre_node)
    {
        generated_node_t node;
        node.name = pre_node.name;
        node.weight = pre_node.weight;

        for(auto const& child : pre_node.children)
        {
            node.add_child(generate_node(child));
        }

        if(pre_node.value_nodes.size() > 0)
        {
            generated_node_t rolled = roll_values(pre_node);
            node.merge_with(rolled);
            ASSERT(node.weight == pre_node.weight, "Node merge affected weight unexpectedly");
        }

        // auto search = pre_node.user_data.find("PrintString");
        if(auto const* nd = find(pre_node.user_data, "PrintString"); nd)
        {
            node.print_string = std::get<std::string>(nd->value);
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
    /// TODO: use regular string concat rather than stringstream
    ///       I'm not using any of the features of SS that make it worth using
    string to_string(user_data_node_t const& node, size_t const depth, size_t level)
    {
        if(level > depth)
            return "";

        stringstream s;

        auto indent = indenation_string(level);

        s << indent;

        if(node.name != "")
            s << node.name_string() << ": ";

        if(is_leaf(node))
        {
            s << node.value;
            return s.str();
        }

        if(level + 1 > depth)
            return s.str();

        for(auto const& child : node.children)
            s << "\n" << to_string(child, depth, level + 1);

        return s.str();
    }

    string to_string(pregen_node_t const& node, size_t const depth, size_t level)
    {
        if(level > depth)
            return "";

        stringstream s;

        auto indent = indenation_string(level);
        
        s << indent;

        if(node.name.size() > 0)
            s << node.name_string();

        if(node.value.index() > 0)
            s << node.value;

        if(node.weight != 1)
            s << " (Weight " + std::to_string(node.weight) + ")";

        s << "\n";


        if(level + 1 > depth)
            return s.str();

        if(node.has_user_data())
            s << to_string(node.user_data, depth, level + 1) << "\n";

        for(auto const& child : node.children)
            s << to_string(child, depth, level + 1);

        indent.append(indenation_string(1));

        
        for(auto const& vnode : node.value_nodes)
            s << to_string(vnode, depth, level + 1);

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
}