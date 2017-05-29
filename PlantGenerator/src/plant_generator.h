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
    using range_value_t = std::vector<std::string>;
    using null_value_t = int;
    
    using variant_value_t = std::variant<std::string, range_value_t, multi_value_t, null_value_t>;

    // The weight value is not a percentage, but rather
    // a multiplier. A value is 'weight' times more likely
    // to be picked (as if it was entered into the value list
    // 'weight' times).
    struct weighted_value_t : public variant_value_t
    {
        using variant_value_t::variant_value_t;

        size_t weight = 1;
    };

    using value_list_t = std::vector<weighted_value_t>;


    /// curiously recurring template pattern (sort-of)
    template <typename T>
    struct base_node_t
    {
        T* parent = nullptr;
        std::vector<T> children;
        std::string name;

        void add_child(T&& c)
        {
            children.push_back(std::move(c));
            children.back().parent = static_cast<T*>(this);
        }

        void add_children(std::vector<T> const& new_children)
        {
            children.insert(children.end(), new_children.begin(), new_children.end());
            for(size_t i = children.size() - new_children.size(); i < children.size(); ++i)
            {
                children[i].parent = static_cast<T*>(this);
            }
        }
    };


    // store value nodeas separately, since I can't store nodes in
    // the variant without them being heap-allocated
    // (since AFAIK if the variant stores a node, which stores the
    // variant, it can lead to an infinite size)
    struct pregen_node_t : base_node_t<pregen_node_t>
    {
        value_list_t values;
        std::vector<pregen_node_t> value_nodes;
        size_t weight = 1;

        void merge_with(pregen_node_t&& n)
        {
            add_children(n.children);
            values.insert(values.end(), n.values.begin(), n.values.end());
            value_nodes.insert(value_nodes.end(), n.value_nodes.begin(), n.value_nodes.end());
        }
    };

    struct generated_node_t : base_node_t<generated_node_t>
    {
        std::vector<std::string> generated_values;

        inline void merge_with(generated_node_t&& n)
        {
            generated_values.insert(generated_values.end(), n.generated_values.begin(), n.generated_values.end());
        }
    };


    /// Functions

    size_t total_weight(value_list_t const& values);
    size_t total_weight(pregen_node_t const& n);
    size_t total_weight(std::vector<pregen_node_t> const& nodes);

    generated_node_t generate_node(pregen_node_t const& node);
    generated_node_t generate_node_from_file(stdfs::path const& filepath);

    generated_node_t roll_values(pregen_node_t const& node);

    void print_node(generated_node_t const& node, size_t level = 0);



    /// Exceptions

    struct file_not_found_exception : public std::runtime_error
    {
        stdfs::path path;

        file_not_found_exception(stdfs::path const& _path)
        : std::runtime_error("File Not Found: \"" + _path.string() + "\"")
        , path(_path)
        {}
    };

    struct include_exception : public std::runtime_error
    {
        stdfs::path include_from_path;
        stdfs::path include_path;

        include_exception(stdfs::path const& _inc_from, stdfs::path const& _inc)
        : std::runtime_error("File Not Found: " + _inc.string() + "\n"
                           + "Included by " + _inc_from.string() + "\n")
        , include_from_path(_inc_from)
        , include_path(_inc)
        {}
    };
}