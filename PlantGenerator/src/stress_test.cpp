#include "stress_test.h"

using namespace std;
using namespace plantgen;
using namespace plant_printer;

int stress_test(generated_node_t const& node, size_t num_iterations)
{
    return stress_test_with_func(
        [&node]()
        {
            print_plant(node);
        }
    , num_iterations);
}

int stress_test(pregen_node_t const& pre_node, size_t num_iterations)
{
    return stress_test_with_func(
        [&pre_node]()
        {
            auto node = generate_node(pre_node);
            node.simplify();
            print_plant(node);
        }
    , num_iterations);
}

int stress_test(std::string const& filepath, size_t num_iterations)
{
    return stress_test_with_func(
        [&filepath]()
        {
            auto pre_node = node_from_file(filepath);
            auto node = generate_node(pre_node);
            node.simplify();
            print_plant(node);
        }
    , num_iterations);
}