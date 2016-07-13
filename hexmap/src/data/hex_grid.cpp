#include "hex_grid.h"


namespace asdf
{
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



    hex_grid_t::hex_grid_t(glm::ivec2 _size)
    : size(_size)
    {
        size_t num_chunks_x = std::ceil((double)size.x / (double)chunk_width);
        size_t num_chunks_y = std::ceil((double)size.y / (double)chunk_height);

        chunks.resize(num_chunks_x);

        for(auto& chunk : chunks)
        {
            chunk.resize(num_chunks_y);
        }
    }
}
}
}