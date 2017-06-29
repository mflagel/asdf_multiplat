#pragma once

#include "plant_generator.h"

namespace plant_printer
{
    std::string print_sub_property(plantgen::generated_node_t const& plant_node);
    std::string print_plant(plantgen::generated_node_t const& plant_node);
}