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
*/


using arg_t = std::pair<std::string, std::string>;
using args_t = std::unordered_map<std::string, std::string>;

arg_t load_arg(int argc, char* argv[], size_t index)
{
    ASSERT(index < argc, "arg index beyond argument count");
    ASSERT(argv[index][0] == '-', "expecting flag specifier to be a dash");
    arg_t arg;

    char asdf_ = argv[index][1];
    arg.first = std::string(asdf_,1); //skip the '-' character

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

    generated_node_t plant;

    try{
        plant = generate_node_from_file(filepath);
    }
    catch(std::runtime_error const& e)
    {
        cout << e.what() << "\n";
    }

    auto search = args.find("p");
    if(search != args.end())
    {
        //print(
    }
    if(args.find("q") == args.end())
    {
        // print(plant);
    }

    //TEST
    auto plant_string = print_plant(plant);
    cout << plant_string;
    //

    return 0;
}