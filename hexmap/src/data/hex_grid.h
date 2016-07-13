#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace asdf
{
namespace hexmap
{
namespace data
{
    constexpr size_t chunk_width  = 10;
    constexpr size_t chunk_height = 10;


    struct hex_grid_chunk_t;

    using hex_tile_id_t = uint32_t;

    struct hex_grid_cell_t
    {
        hex_tile_id_t tile_id = 0;
    };

    
    using hex_cells_t = std::vector<std::vector<hex_grid_cell_t>>;

    struct hex_grid_chunk_t
    {
        hex_cells_t cells;

        hex_grid_chunk_t(glm::ivec2 size = glm::ivec2(chunk_width, chunk_height));
    };


    using hex_chunks_t = std::vector<std::vector<hex_grid_chunk_t>>;

    struct hex_grid_t
    {
        glm::ivec2 size;
        hex_chunks_t chunks;

        hex_grid_t(glm::ivec2 size);
    };
}
}
}