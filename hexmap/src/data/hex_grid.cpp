#include "hex_grid.h"


namespace hexmap
{
namespace data
{
    hex_grid_chunk_t::hex_grid_chunk_t(glm::ivec2 size)
    {
        cells.resize(size.x);

        for(auto& grid_y : cells)
        {
            grid_y.resize(size.y);
        }
    }



    hex_grid_t::hex_grid_t(glm::ivec2 size)
    {
    }
}
}