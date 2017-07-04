#include <array>
#include <iostream>
#include <unordered_map>
#include <string>

#include "asdf_multiplat/main/asdf_defs.h"
#include "asdf_multiplat/utilities/utilities.h"

#include "plant_generator.h"
#include "plant_printer.h"
#include "stress_test.h"

using namespace plantgen;
using namespace plant_printer;
using namespace asdf::util;

// undef main because apparently SDL redefines 'main' as SDL_main
// which is super obnoxious
#undef main


/*  Possible Args *
-q  : print nothing
-p  : print pregen nodes
-g  : print generated nodes
-s  : print simplified (ie a plain-english description using PlantPrinter)
-t  : stress test
*/

enum flag_e
{
    flag_none              = 0
  , flag_quiet             = 1
  , flag_print_pregen      = 2
  , flag_print_generated   = 4
  , flag_print_simplified  = 8
  , flag_stress_test       = 16
  , flag_all               = 0xFFFFFFFF
};

using str_lit_vec = std::initializer_list<char const*>; //vector of string literals (stored as char const*)
constexpr std::initializer_list<str_lit_vec> flag_string_sets =
{
      str_lit_vec{}
    , str_lit_vec{"-q", "--quiet"}
    , str_lit_vec{"-p", "--print-pregen"}
    , str_lit_vec{"-g", "--print-generated"}
    , str_lit_vec{"-s", "--print-simplified"}
    , str_lit_vec{"-t", "--stress_test"}
};


using arg_t = std::pair<std::string, std::string>;
using args_t = std::unordered_map<std::string, std::string>;
using flags_t = uint32_t;

arg_t load_arg(int argc, char* argv[], size_t index)
{
    ASSERT(index < argc, "arg index beyond argument count");
    ASSERT(argv[index][0] == '-', "expecting flag specifier to be a dash");
    arg_t arg;

    arg.first = std::string(argv[index]);

    //if the next argv doesnt start with a dash
    if(index+1 < argc && argv[index+1][0] != '-')
        arg.second = argv[index+1];

    return arg;
}

args_t load_args(int argc, char* argv[])
{
    args_t args;

    for(size_t i = 1; i < argc; ++i)
    {
        if(argv[i][0] == '-')
            args.insert(load_arg(argc, argv, i));
    }

    return args;
}

flags_t load_flags(args_t const& args)
{
    flags_t flags;

    for(auto const& arg : args)
    {
        //search flag_strings for arg
        int flag_set_index = 0;
        for(auto const& flag_strings: flag_string_sets)
        {
            for(auto const& s : flag_strings)
            {
                if(str_eq(arg.first.c_str(), s))
                {
                    flags |= 1 << (flag_set_index - 1);
                    break;
                }
            }
            
            ++flag_set_index;
        }
    }

    return flags;
}




int main(int argc, char* argv[])
{
    using namespace std;
    if(argc < 2)
    {
        cout << "TODO: output helpful message when there are no arguments";
        return 0;
    }

    args_t args = load_args(argc, argv);
    flags_t flags = load_flags(args);

    std::string filepath = std::string(argv[argc-1]);

    pregen_node_t pregen_node;
    generated_node_t generated_node;

    try{
        pregen_node = node_from_file(filepath);
        generated_node = generate_node(pregen_node);
    }
    catch(std::runtime_error const& e)
    {
        cout << e.what() << "\n";
        return 1;
    }

    
    // if no flags, default to print_simplified
    // set default before flags is captured in lambda below
    flags |= (flag_print_simplified * (flags == 0));


    auto has_flag = [flags](uint _flags) -> bool
    {
        return (flags & _flags) > 0;
    };

    if(has_flag(flag_stress_test))
    {
        cout << "testing...";
        size_t num_iterations = 1000;
        for(size_t i = 0; i < num_iterations; ++i)
        {
            cout << "\rtesting  [" << i << "/" << num_iterations << "]" << std::flush;
            // int test_result = stress_test(generated_node, 1);
            // int test_result = stress_test(pregen_node, 1);
            int test_result = stress_test(filepath, 1);

            if(test_result > 0)
            {
                cout << "\n";
                return test_result;
            }
        }

        cout << "\rtesting  [100/100]\nno exceptions\n";
    }


    if(!has_flag(flag_quiet))
    {
        if(has_flag(flag_print_pregen))
        {
            print_node(pregen_node);
        }
        if(has_flag(flag_print_generated))
        {
            print_node(generated_node);
        }
        if(has_flag(flag_print_simplified))
        {
            generated_node.simplify();
            auto plant_string = print_plant(generated_node);
            cout << plant_string;
        }

        cout << "\n";
    }


    return 0;
}