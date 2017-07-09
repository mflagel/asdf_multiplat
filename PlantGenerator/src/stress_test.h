#pragma once

#include <iostream>
#include <string>

#include "plant_generator.h"
#include "plant_printer.h"

template<typename F>
int stress_test_with_func(F&& test_function, size_t num_iterations)
{
    try{
        for(size_t i = 0; i < num_iterations; ++i)
        {
            test_function();
        }
    } catch(std::exception const& e)
    {
        std::cout << "an exception happened\n";
        std::cout << e.what() << "\n";
        return 1;
    }
    return 0;
}


int stress_test(plantgen::generated_node_t const& node,  size_t num_iterations);
int stress_test(plantgen::pregen_node_t const& pre_node, size_t num_iterations);
int stress_test(std::string const& filepath,   size_t num_iterations);