#pragma once

#include <vector>
#include <string>
#include <stdint.h>

#include <glm/glm.hpp>

namespace asdf
{
namespace hexmap
{

    glm::ivec2 world_to_hex_coord(glm::vec2 world_pos);

namespace data
{
    constexpr size_t new_chunk_width  = 10;
    constexpr size_t new_chunk_height = 10;


    struct hex_grid_chunk_t;

    using hex_tile_id_t = uint32_t;

    struct hex_grid_cell_t
    {
        hex_tile_id_t tile_id = 1; //temp setting as 1 to for saving/loading
    };

    
    using hex_cells_t = std::vector<std::vector<hex_grid_cell_t>>; //row of columns

    struct hex_grid_chunk_t
    {
        glm::ivec2 position; //in chunk units
        glm::uvec2 size; // how much of the allocated memory is actually used
        glm::uvec2 allocated_size; //how much memory is actually allocated

        hex_cells_t cells;

        hex_grid_chunk_t(glm::uvec2 size = glm::uvec2(new_chunk_width, new_chunk_height));

        hex_grid_cell_t& cell_at_local_coord(glm::ivec2 c) {return cells[c.x][c.y];}
    };

    using hex_chunks_t = std::vector<std::vector<hex_grid_chunk_t>>; // row of columns

    enum resize_x_direction_e
    {
          resize_leftwards
        , resize_rightwards
    };

    enum resize_y_direction_e
    {
          resize_upwards
        , resize_downwards
    };

    struct hex_grid_t
    {
        glm::uvec2 size;
        hex_chunks_t chunks;

        size_t num_chunks() const { return size.x * size.y; }

        /// WARNING: Messing with this function will affect saving/loading
        ///          Chunks may not load the same order they were saved
        template<typename F>
        void for_each_chunk(F f)
        {
            for(size_t x = 0; x < chunks.size(); ++x)
            {
                for(size_t y = 0; y < chunks[x].size(); ++y)
                {
                    chunks[x][y].position = glm::ivec2(x,y);  //.. this might not be the best place for this
                    f(chunks[x][y]);
                }
            }
        }

        hex_grid_t(glm::uvec2 size);
        hex_grid_t(std::string const& filepath);

        void init(glm::uvec2 size, glm::uvec2 chunk_size = glm::uvec2(new_chunk_width, new_chunk_height));

        void save_to_file(std::string const& filepath);
        void load_from_file(std::string const& filepath);

        void resize_by_copy(glm::ivec2 new_size, resize_x_direction_e, resize_y_direction_e);

        void resize(glm::uvec2 new_size, resize_x_direction_e, resize_y_direction_e);
        void grow(glm::uvec2 grow_amount, resize_x_direction_e, resize_y_direction_e);
        void shrink(glm::uvec2 shrink_amount, resize_x_direction_e, resize_y_direction_e);

        void add_columns(size_t num_cols, resize_x_direction_e = resize_rightwards);
        void add_rows(size_t num_rows, resize_y_direction_e = resize_upwards);

        bool is_in_bounds(glm::ivec2 hex_coords) const;

        glm::uvec2 chunk_size() const;
        hex_grid_cell_t& cell_at(glm::ivec2 hex_coord);
        hex_grid_cell_t& cell_at(glm::vec2 world_pos);
        glm::ivec2 chunk_coord_from_hex_coord(glm::ivec2) const;
        hex_grid_chunk_t& chunk_from_hex_coord(glm::ivec2);
    };



    struct hxm_header_t
    {
        size_t version = size_t(-1);

        glm::uvec2 map_size;
        glm::uvec2 chunk_size;
    };
}
}
}