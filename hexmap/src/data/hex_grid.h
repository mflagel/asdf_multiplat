#pragma once

#include <vector>

#include <glm\glm.hpp>

namespace hexmap
{
namespace data
{
    struct hex_grid_chunk_t;

    struct hex_grid_cell_t
    {
        size_t tile_id = 0;
    };

    
    using hex_cells_t = std::vector<std::vector<hex_grid_cell_t>>;

    struct hex_grid_chunk_t
    {
        hex_cells_t cells;

        hex_grid_chunk_t(glm::ivec2 size);
    };


    using hex_chunks_t = std::vector<std::vector<hex_grid_chunk_t>>;

    struct hex_grid_t
    {
        hex_chunks_t chunks;

        hex_grid_t(glm::ivec2 size);
    };
}
}