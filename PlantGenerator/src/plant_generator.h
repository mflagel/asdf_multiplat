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
        T* parent = nullptr;
        std::vector<T> children;
        std::vector<T> value_nodes;

        std::string name;
        std::string sub_name; //used for includes

        uint32_t weight = 1;

        base_node_() = default;
        base_node_(std::string const& _name, uint32_t _weight = 0)
        : name(_name)
        , weight(_weight)
        {}

        std::string name_string() const
        {
            std::string s = name;
            if(sub_name.size() > 0)
                s += " (" + sub_name + ")";

            return s;
        }

        size_t child_index(std::string const& child_name) const
        {
            ///TODO: investigate if it's faster to compare hashes
            for(size_t i = 0; i < children.size(); ++i)
                if(children[i].name == child_name)
                    return i;

            throw std::out_of_range(std::string("no child found named '" + child_name + "' in '" + name + "'"));
        }

        T& child(std::string const& child_name)
        {
            return children[child_index(child_name)];
        }

        T const& child(std::string const& child_name) const
        {
            return children[child_index(child_name)];
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

    struct user_data_node_t : base_node_<user_data_node_t>
    {
        user_value_t value;

        user_data_node_t() = default;

        user_data_node_t(std::string const& _name, user_value_t const& _uval)
        : base_node_<user_data_node_t>(_name)
        , value(_uval)
        {
        }

        void merge_with(user_data_node_t const& n)
        {
            base_node_::merge_with(n);
            value = n.value;
        }

        template <typename T>
        T& value_as()
        {
            return std::get<T>(value);
        }
        template <typename T>
        T const& value_as() const
        {
            return std::get<T>(value);
        }
    };


    struct pregen_node_t : base_node_<pregen_node_t>
    {
        variant_value_t value;

        user_data_node_t user_data;

        pregen_node_t()
        : base_node_<pregen_node_t>()
        {
            user_data.name = "User Data";
        }

        pregen_node_t(weighted_value_t const& wv)
        : base_node_<pregen_node_t>("", wv.weight)
        , value(wv)
        {
            user_data.name = "User Data";
        }

        void merge_with(pregen_node_t const& n)
        {
            base_node_::merge_with(n);
            value = n.value;
            user_data.merge_with(n.user_data);
        }

        bool has_user_data() const
        { return user_data.children.size() > 0; }

        variant_value_t const& value_at(size_t i) const
        {
            ASSERT(i < value_nodes.size(), "");
            return value_nodes[i].value;
        }

        template <typename T>
        T const& value_at(size_t i) const
        {
            ASSERT(i < value_nodes.size(), "");
            return std::get<T>(value_nodes[i].value);
        }


        template <typename T>
        T const& value_as() const
        {
            return std::get<T>(value);
        }
    };


    struct generated_node_t : base_node_<generated_node_t>
    {
        std::vector<std::string> generated_values;
        std::string print_string;

        size_t num_rollable_values = nullindex;
        size_t value_index = nullindex;

        using base_node_::base_node_;

        inline std::string const& generated_value() const
        {
            ASSERT(generated_values.size() > 0, "Cannot grab a generated value from an empty list");
            WARN_IF(generated_values.size() > 1, "There are generated values being ignored from %s", name.c_str());
            return generated_values[0];
        }

        inline void merge_with(generated_node_t const& n)
        {
            base_node_::merge_with(n);
            generated_values.insert(generated_values.end(), n.generated_values.begin(), n.generated_values.end());

            if(num_rollable_values == nullindex)
                num_rollable_values = n.num_rollable_values;

            WARN_IF(value_index != nullindex && n.value_index != nullindex
                  , "generated node merge conflict (%s and %s; both nodes have a value index) (%zu and %zu)"
                  , name.c_str(), n.name.c_str(), value_index, n.value_index);
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
    /// FIXME: return ref and throw exception on not found??
    ///        have a similar find_if<>
    ///        similar to std:get<> and std::get_if<>
    ///        in face maybe this should be named get()
    template <typename Node>
    Node const* find(Node const& node, std::string name)
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

    template <typename T>
    bool is_leaf(base_node_<T> const& n)
    {
        return n.children.empty() && n.value_nodes.empty();
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
        return n->generated_value();
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


    pregen_node_t    node_from_file(stdfs::path const& filepath);
    generated_node_t generate_node(pregen_node_t const& node);
    generated_node_t generate_node_from_file(stdfs::path const& filepath);

    generated_node_t roll_values(pregen_node_t const& node);
    std::vector<std::string> roll_multi_value(multi_value_t const& m);

    using sz = std::numeric_limits<size_t>;

    std::string indenation_string(size_t indent_level);
    std::string to_string(user_data_node_t const& node, size_t depth = sz::max(), size_t level = 0);
    std::string to_string(pregen_node_t    const& node, size_t depth = sz::max(), size_t level = 0);
    std::string to_string(generated_node_t const& node, size_t depth = sz::max(), size_t level = 0);

    void print_node(pregen_node_t    const& node, size_t depth = sz::max(), size_t level = 0);
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