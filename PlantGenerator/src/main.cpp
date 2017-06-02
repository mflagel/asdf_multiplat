#include <iostream>

#include "plant_generator.h"

using namespace plantgen;

// undef main because apparently SDL redefines 'main' as SDL_main
// which is super obnoxious
#undef main

int main(int argc, char* argv[])
{
    using namespace std;
    if(argc < 2)
    {
        cout << "TODO: output helpful message when there are no arguments";
        return 0;
    }

    std::string filepath = std::string(argv[1]);

    try{
        generated_node_t plant = generate_node_from_file(filepath);
        print_node(plant);
    }
    catch(std::runtime_error const& e)
    {
        cout << e.what() << "\n";
    }

    return 0;
}