#pragma once

#include <unordered_map>

#include "plant_generator.h"

namespace plant_printer
{
    using string_var_locations_t = std::map<std::string, int>;

    enum print_flags_e
    {
        print_flags_none = 0
      , print_flags_skip_name = 1
    };
    using print_flags_t = uint32_t;

    string_var_locations_t parse_print_string(std::string const& print_string);

    std::string print_property_with_string(plantgen::generated_node_t const& plant_node, std::string const& print_string);
    std::string print_property_with_string(plantgen::generated_node_t const& node);

    std::string print_sub_property(plantgen::generated_node_t const& plant_node, size_t level = 0, print_flags_t print_flags = print_flags_none);
    std::string print_plant(plantgen::generated_node_t const& plant_node);
}