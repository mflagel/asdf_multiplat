#pragma once

#include <vector>
#include <string>
#include <stdint.h>

#include <glm/glm.hpp>

struct SDL_RWops;

namespace asdf
{
namespace hexmap
{

    glm::ivec2 world_to_hex_coord(glm::vec2 world_pos);
    glm::vec2 hex_to_world_coord(glm::ivec2 hex_coord, bool odd_q = false);

namespace data
{
    struct hxm_header_t;

    constexpr size_t new_chunk_width  = 10;
    constexpr size_t new_chunk_height = 10;


    struct hex_grid_chunk_t;

    using hex_tile_id_t = uint32_t;

    struct hex_grid_cell_t
    {
        hex_tile_id_t tile_id;
    };

    inline bool operator==(hex_grid_cell_t const& lhs, hex_grid_cell_t const& rhs)
    {
        return lhs.tile_id == rhs.tile_id;
    }

    inline bool operator!=(hex_grid_cell_t const& lhs, hex_grid_cell_t const& rhs)
    {
        return !(lhs == rhs);
    }

    //FIXME see if something like this exists already
    struct bbox_units_t
    {
        glm::vec2 lower;
        glm::vec2 upper;
    };

    
    // row of columns ( accessed [x,y] )
    // iterated for(each row in cells){ 
    //              for(each x in row)
    //              { ...stuff... }
    using hex_cells_t = std::vector<std::vector<hex_grid_cell_t>>;

    struct hex_grid_chunk_t
    {
        glm::ivec2 position; //in chunk units
        glm::uvec2 size; // how much of the allocated memory is actually used
        glm::uvec2 allocated_size; //how much memory is actually allocated

        hex_cells_t cells;

        hex_grid_chunk_t(glm::uvec2 size = glm::uvec2(new_chunk_width, new_chunk_height), hex_grid_cell_t const& default_cell_style = hex_grid_cell_t{0});

        bool operator==(hex_grid_chunk_t const&) const;
        bool operator!=(hex_grid_chunk_t const& rhs) const { return !(*this == rhs); }
        bool contents_equal(hex_grid_chunk_t const&) const;

        inline hex_grid_cell_t& cell_at_local_coord(glm::ivec2 c) {return cells[c.x][c.y];}
        inline hex_grid_cell_t& cell_at_local_coord(int x, int y) {return cells[x][y];}

        inline hex_grid_cell_t const& cell_at_local_coord(glm::ivec2 c) const {return cells[c.x][c.y];}
        inline hex_grid_cell_t const& cell_at_local_coord(int x, int y) const {return cells[x][y];}
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
        glm::uvec2 size; //size in number of cells (ie: the size of the grid)
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

        hex_grid_t(glm::uvec2 size, hex_grid_cell_t const& default_cell_style = hex_grid_cell_t{});

        void init(glm::uvec2 size, glm::uvec2 chunk_size = glm::uvec2(new_chunk_width, new_chunk_height), hex_grid_cell_t const& default_cell_style = hex_grid_cell_t{});
        void init(glm::uvec2 size, hex_grid_cell_t const& default_cell_style);

        void save_to_file(SDL_RWops*); //TODO: look into not_null pointers from C++17
        void load_from_file(SDL_RWops*, hxm_header_t const&);

        void resize_by_copy(glm::ivec2 new_size, resize_x_direction_e, resize_y_direction_e);

        void resize(glm::uvec2 new_size, resize_x_direction_e, resize_y_direction_e);
        void grow(glm::uvec2 grow_amount, resize_x_direction_e, resize_y_direction_e);
        void shrink(glm::uvec2 shrink_amount, resize_x_direction_e, resize_y_direction_e);

        void add_columns(size_t num_cols, resize_x_direction_e = resize_rightwards);
        void add_rows(size_t num_rows, resize_y_direction_e = resize_upwards);

        bool is_in_bounds(glm::ivec2 hex_coords) const;
        bbox_units_t bounding_box_units() const;

        inline glm::uvec2 size_cells() const { return size; }
        glm::vec2 size_units() const;
        glm::uvec2 size_chunks() const;

        glm::uvec2 chunk_size() const;
        hex_grid_cell_t& cell_at(glm::ivec2 hex_coord);
        hex_grid_cell_t& cell_at(glm::vec2 world_pos);
        glm::ivec2 chunk_coord_from_hex_coord(glm::ivec2) const;
        hex_grid_chunk_t& chunk_from_hex_coord(glm::ivec2);
    };
}
}
}