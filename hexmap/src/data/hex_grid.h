#pragma once

#include <vector>
#include <stdint.h>

#include <glm/glm.hpp>

namespace asdf
{
namespace hexmap
{
namespace data
{
    constexpr size_t max_chunk_width  = 10;
    constexpr size_t max_chunk_height = 10;


    struct hex_grid_chunk_t;

    using hex_tile_id_t = uint32_t;

    struct hex_grid_cell_t
    {
        hex_tile_id_t tile_id = 0;
    };

    
    using hex_cells_t = std::vector<std::vector<hex_grid_cell_t>>;

    struct hex_grid_chunk_t
    {
        glm::ivec2 position; //in chunk units
        glm::uvec2 size; // how much of the allocated memory is actually used
        glm::uvec2 allocated_size; //how much memory is actually allocated

        hex_cells_t cells;

        hex_grid_chunk_t(glm::uvec2 size = glm::uvec2(max_chunk_width, max_chunk_height));

        hex_grid_cell_t& cell_at_local_coord(glm::ivec2 c) {return cells[c.x][c.y];}
    };


    using hex_chunks_t = std::vector<std::vector<hex_grid_chunk_t>>;

    struct hex_grid_t
    {
        glm::ivec2 size;
        hex_chunks_t chunks;

        size_t num_chunks() const { return size.x * size.y; }

        template<typename F>
        void for_each_chunk(F f)
        {
            for(size_t x = 0; x < chunks.size(); ++x)
            {
                for(size_t y = 0; y < chunks[x].size(); ++y)
                {
                    chunks[x][y].position = glm::ivec2(x,y);
                    f(chunks[x][y]);
                }
            }
        }

        hex_grid_t(glm::ivec2 size);

        bool is_in_bounds(glm::ivec2 hex_coords) const;

        hex_grid_cell_t& cell_at(glm::ivec2 hex_coord);
        glm::ivec2 chunk_coord_from_hex_coord(glm::ivec2) const;
        hex_grid_chunk_t& chunk_from_hex_coord(glm::ivec2);
    };
}
}
}