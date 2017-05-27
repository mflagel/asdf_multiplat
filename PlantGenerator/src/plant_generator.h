#pragma once

#ifdef _MSC_VER
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace stdfs = std::experimental::filesystem;

namespace plantgen
{
    
    struct multi_value_t
    {
        size_t num_to_pick;

        std::vector<std::string> values;
    };


    using null_value_t = int;
    using range_value_t = std::vector<std::string>;
    using variant_value_t = std::variant<std::string, range_value_t, multi_value_t, null_value_t>;
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
    };

    void generate_node(node_t& node);
    node_t generate_plant_from_file(stdfs::path const& filepath);

    void print_node(node_t const& node, size_t level);
    void print_plant(node_t& plant);
}