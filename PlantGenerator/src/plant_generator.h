#pragma once

#ifdef _MSC_VER
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include <limits>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>

#include <asdf_multiplat/main/asdf_defs.h>

#include "value_types.hpp"

namespace stdfs = std::experimental::filesystem;

namespace plantgen
{
    constexpr int weight_inherit_code = -9001; //todo: better number?

    /// curiously recurring template pattern (sort-of)
    template <typename T>
    struct base_node_
    {
        std::string name;
        std::string sub_name; //used for includes

        uint32_t weight = 1;

        T* parent = nullptr;
        std::vector<T> children;
        std::vector<T> value_nodes;

        base_node_() = default;
        base_node_(std::string const& _name)
        : name(_name)
        {}

        std::string name_string() const
        {
            std::string s = name;
            if(sub_name.size() > 0)
                s += " (" + sub_name + ")";

            return s;
        }

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

        void add_value_node(T&& vn)
        {
            value_nodes.push_back(std::move(vn));
            value_nodes.back().parent = static_cast<T*>(this);
        }

        void merge_with(T const& n)
        {
            if(name != n.name)
                sub_name = n.name;

            if(n.weight == weight_inherit_code)
                weight = weight_inherit_code;
            else
                weight = std::max(weight, n.weight); //not sure if I like this

            add_children(n.children);
            value_nodes.insert(value_nodes.end(), n.value_nodes.begin(), n.value_nodes.end());
        }

        void simplify(size_t depth = 0, size_t level = 0)
        {
            if(depth > 0 && level > depth)
                return;

            if(children.empty() && value_nodes.size() == 1)
            {
                static_cast<T*>(this)->merge_with(value_nodes[0]);
                value_nodes.clear();
            }

            for(auto& child : children)
                child.simplify(depth, level + 1);
            for(auto& vn : value_nodes)
                vn.simplify(depth, level + 1);
        }
    };


    // store value nodeas separately, since I can't store nodes in
    // the variant without them being heap-allocated
    // (since AFAIK if the variant stores a node, which stores the
    // variant, it can lead to an infinite size)
    struct pregen_node_t : base_node_<pregen_node_t>
    {
        value_list_t values;

        user_data_t user_data;

        using base_node_::base_node_;

        void merge_with(pregen_node_t const& n)
        {
            base_node_::merge_with(n);
            values.insert(values.end(), n.values.begin(), n.values.end());
            user_data.insert(n.user_data.begin(), n.user_data.end());
        }
    };

    struct generated_node_t : base_node_<generated_node_t>
    {
        std::vector<std::string> generated_values;
        std::string print_string;

        size_t num_rollable_values = nullindex;
        size_t num_rollable_value_nodes = nullindex;
        size_t value_index = nullindex;

        using base_node_::base_node_;

        size_t num_rollable_values_and_vnodes() const
        { return num_rollable_values + num_rollable_value_nodes; }

        inline void merge_with(generated_node_t const& n)
        {
            base_node_::merge_with(n);
            generated_values.insert(generated_values.end(), n.generated_values.begin(), n.generated_values.end());

            if(num_rollable_values == nullindex)
                num_rollable_values = n.num_rollable_values;
            if(num_rollable_value_nodes == nullindex)
                num_rollable_value_nodes = n.num_rollable_value_nodes;


            //ASSERT(value_index == nullindex || n.value_index == nullindex, "node merge conflict: value index");
            WARN_IF(value_index != nullindex && n.value_index != nullindex, "generated node merge conflict (both nodes have a value index) (%zu and %zu)", value_index, n.value_index);
            if(value_index == nullindex)
                value_index = n.value_index;

            WARN_IF(print_string.size() > 0 && n.print_string.size() > 0, "generated node merge conflict (both nodes have a print string) (%s and %s)", name, n.name);
            if(print_string.empty())
                print_string = n.print_string;
        }
    };


    /// Template Functions ///


    /// A lazy breadth-first depth-second hybrid
    /// TODO: implement a real bfs
    template <typename T>
    base_node_<T> const* find(base_node_<T> const& node, std::string name)
    {
        for(auto const& child : node.children)
        {
            if(child.name == name)
                return &child;
        }

        for(auto const& child : node.children)
        {
            auto* pn = find(child, name);
            if(pn)
                return pn;
        }

        return nullptr;
    }


    template <typename Node>
    Node const* leaf_value_node(Node const& node)
    {
        if(node.value_nodes.empty())
            return &node;
        else
            return leaf_value_node(node.value_nodes[0]);
    }


    inline std::string const& leaf_value_name(generated_node_t const& node)
    {
        auto const* n = leaf_value_node(node);
        ASSERT(n, "Unexpected null node");
        ASSERT(n->generated_values.size() > 0, "Expected a generated value");
        return n->generated_values[0];
    }

    inline std::string const& value_name(generated_node_t const& node)
    {
        if(node.generated_values.size() > 0)
            return  node.generated_values[0];
        else if(node.value_nodes.size() > 0)
            return node.value_nodes[0].name;
        else
        {
            EXPLODE("Expected a generated value or a value node");
            return node.name;
        }
    }


    /// Function Declarations ///
    int32_t random_int(uint32_t min, uint32_t max);
    int32_t random_int(uint32_t max);


    template <typename L>
    uint32_t total_weight(L const& list);

    std::string indenation_string(size_t indent_level);


    pregen_node_t    node_from_file(stdfs::path const& filepath);
    generated_node_t generate_node(pregen_node_t const& node);
    generated_node_t generate_node_from_file(stdfs::path const& filepath);

    generated_node_t roll_values(pregen_node_t const& node);

    using sz = std::numeric_limits<size_t>;

    std::string to_string(pregen_node_t const& node, size_t depth = sz::max(), size_t level = 0);
    std::string to_string(generated_node_t const& node, size_t depth = sz::max(), size_t level = 0);

    void print_node(pregen_node_t const& node, size_t depth = sz::max(), size_t level = 0);
    void print_node(generated_node_t const& node, size_t depth = sz::max(), size_t level = 0);


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