#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace plantgen
{
    struct value_range_t
    {
        struct entry_t
        {
            float weight;
            std::string name;
        };

        std::vector<entry_t> entries;
    };

    struct multi_value_t
    {
        size_t num_to_pick;

        std::vector<std::string> values;
    };


    using null_value_t = int;
    using variant_value_t = std::variant<std::string, value_range_t, multi_value_t, null_value_t>;
    using value_list_t = std::vector<variant_value_t>;

    struct node_t
    {
        node_t* parent = nullptr;
        std::vector<node_t> children;

        std::string name;
        value_list_t values;
        std::vector<std::string> generated_values;

        node_t() = default;
        node_t(std::string _name)
        : name(std::move(_name))
        {}

        inline bool is_leaf() const { return children.empty(); }

        //void add_entry(node_t&&);
    };

    void generate_node(node_t& node);
    node_t generate_plant_from_file(std::string const& filepath);

    void print_node(node_t const& node, size_t level);
    void print_plant(node_t& plant);
}