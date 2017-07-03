#include <iostream>
#include <unordered_map>
#include <string>

#include "asdf_multiplat/main/asdf_defs.h"

#include "plant_generator.h"
#include "plant_printer.h"

using namespace plantgen;
using namespace plant_printer;

// undef main because apparently SDL redefines 'main' as SDL_main
// which is super obnoxious
#undef main


/*  Possible Args *
-q  : print nothing
-p  : print pregen nodes
-g  : print generated nodes
*/


using arg_t = std::pair<std::string, std::string>;
using args_t = std::unordered_map<std::string, std::string>;

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

args_t load_flags(int argc, char* argv[])
{
    args_t args;

    for(size_t i = 1; i < argc; ++i)
    {
        if(argv[i][0] == '-')
            args.insert(load_arg(argc, argv, i));
    }

    return args;
}

int main(int argc, char* argv[])
{
    using namespace std;
    if(argc < 2)
    {
        cout << "TODO: output helpful message when there are no arguments";
        return 0;
    }

    auto args = load_flags(argc, argv);

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
    }

    //if no quiet flag
    if(args.find("-q") == args.end())
    {
        bool output_pregen  = args.find("-p") != args.end();
        bool output_postgen = args.find("-g") != args.end();

        if(output_pregen)
        {
            print_node(pregen_node);
        }
        if(output_postgen)
        {
            print_node(generated_node);
        }

        if(!output_pregen && !output_postgen)
        {
            auto plant_string = print_plant(generated_node);
            cout << plant_string;
        }

        cout << "\n";
    }

    return 0;
}