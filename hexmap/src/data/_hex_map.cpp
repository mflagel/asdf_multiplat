#include "stdafx.h"
#include "hex_map.h"


namespace asdf
{
namespace hexmap
{
namespace data
{
    hex_map_t::hex_map_t(glm::uvec2 grid_size)
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


    size_t hex_map_t::object_index_at(glm::vec2 world_pos)
    {
        size_t obj_index = 0;
        for(auto const& obj : objects)
        {
            auto ub = obj.position + obj.size_d2;
            auto lb = obj.position - obj.size_d2;

            if(world_pos.x >= lb.x && world_pos.x <= ub.x &&
               world_pos.y >= lb.y && world_pos.y <= ub.y)
            {
                return obj_index;
            }

            ++obj_index;
        }

        return -1;
    }

}
}
}