#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace plantgen
{
    //using node_type_t = std::variant<range_t>

    struct node_t
    {
        node_t* parent = nullptr;
        std::vector<node_t> children;

        std::string name;
        std::vector<std::string> values;

        node_t(std::string name);

        inline bool is_leaf() const { return children.empty(); }

        //void add_entry(node_t&&);
    };

    node_t load_params(std::string const& filepath);
    node_t generate_plant(std::string const& filepath);
}