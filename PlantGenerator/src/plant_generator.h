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


    /// curiously recurring template pattern
    template <typename T>
    struct base_node_t
    {
        T* parent = nullptr;
        std::vector<T> children;
        std::string name;
    };


    // store value nodeas separately, since I can't store nodes in
    // the variant without them being heap-allocated
    // (since AFAIK if the variant stores a node, which stores the
    // variant, it can lead to an infinite size)
    struct pregen_node_t : base_node_t<pregen_node_t>
    {
        value_list_t values;
        std::vector<pregen_node_t> value_nodes;
    };

    struct generated_node_t : base_node_t<generated_node_t>
    {
        std::vector<std::string> generated_values;
    };


    generated_node_t generate_node(pregen_node_t& node);
    generated_node_t generate_node_from_file(stdfs::path const& filepath);

    std::vector<std::string> roll_values(pregen_node_t const& node);

    void print_node(generated_node_t const& node, size_t level = 0);
}