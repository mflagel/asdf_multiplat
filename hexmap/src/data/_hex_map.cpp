#include "stdafx.h"
#include "hex_map.h"


namespace asdf
{
namespace hexmap
{
namespace data
{
    hex_map_t::hex_map_t(glm::ivec2 grid_size)
    : hex_grid(grid_size)
    {
    }

    //TODO: refactor save/load code such that hex_grid_t::save_to_file() takes a SDL_RWops*
    //      to read/write to

    void hex_map_t::save_to_file(std::string const& filepath)
    {
        hex_grid.save_to_file(filepath);
    }

    void hex_map_t::load_from_file(std::string const& filepath)
    {
        hex_grid.load_from_file(filepath);
    }

}
}
}