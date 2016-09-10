#include "stdafx.h"
#include "hex_grid.h"


using namespace std;
using namespace glm;

namespace asdf
{
namespace hexmap
{
namespace data
{
    hex_grid_chunk_t::hex_grid_chunk_t(glm::uvec2 _size)
    : size(_size)
    , allocated_size(_size)
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
        add_columns(size.x);
        add_rows(size.y);

        /*
        auto dv_x = div(size.x, max_chunk_width);
        auto dv_y = div(size.y, max_chunk_height);

        size_t num_chunks_x = dv_x.quot + (dv_x.rem > 0);
        size_t num_chunks_y = dv_y.quot + (dv_y.rem > 0);

        chunks.resize(num_chunks_x); //create the top row

        //cycle through top row and add columns
        for(size_t x = 0; x < num_chunks_x; ++x)
        {
            chunks[x].resize(num_chunks_y); //add chunks for this column

            //for each chunk, set position and handle odd edges
            for(size_t y = 0; y < num_chunks_y; ++y)
            {
                //set position
                chunks[x][y].position = ivec2(x,y);

                //adjust the size of everything on the furthest column
                if(dv_x.rem > 0 && x == num_chunks_x - 1)
                    chunks[x][y].size.x = dv_x.rem;
            }

            //adjust the size of the furthest cells of the row if necessary
            if(dv_y.rem > 0)
                chunks[x][chunks[x].size() - 1].size.y = dv_y.rem;
        }
        */
    }

    /// Resize by allocating an entirely new hex-grid and copying the data over
    void resize_by_copy(glm::ivec2 new_size, resize_x_direction_e, resize_y_direction_e)
    {
        
    }

    void hex_grid_t::resize(glm::ivec2 new_size, resize_x_direction_e xdir, resize_y_direction_e ydir)
    {
        if(new_size.x > size.x && new_size.y > size.y)  //grow both
        {
            grow(new_size - old_size, xdir, ydir);
        }
        else if(new_size.x < size.x && new_size.y < size.y)  //shrink both
        {
            shrink(new_size - old_size, xdir, ydir);
        }
        else if(new_size.x > size.x && new_size.y < size.y)  //shrink y, grow x
        {
            shrink(ivec2(size.x, size.y - new_size.y), xdir, ydir);   //shrink before grow to minimize data storage needed to undo such an action
            grow(ivec2(new_size.x - size.x, size.y), xdir, ydir);
        }
        else if(new_size.x < size.x && new_size.y > size.y)  //shrink x, grow y
        {
            shrink(ivec2(size.x - new_size.x, size.y), xdir, ydir);
            grow(ivec2(size.x, new_size.y - size.y), xdir, ydir);
        }
    }

    void hex_grid_t::grow(glm::ivec2 grow_amount, resize_x_direction_e xdir, resize_y_direction_e ydir)
    {
        ASSERT(grow_amount.x >=0 && grow_amount.y >= 0, "");

        if(grow_amount.x > 0)
        {
            add_columns(grow_amount.x, xdir);
        }
        if(grow_amount.y > 0)
        {
            add_rows(grow_amount.y, ydir);
        }
    }

    void hex_grid_t::shrink(glm::ivec2 shrink_amount, resize_x_direction_e, resize_y_direction_e)
    {
        ASSERT(shrink_amount.x >=0 && shrink_amount.y >= 0, "");
        EXPLODE("todo: hex_grid_t::shrink()");
    }


    // WIP
    void add_columns(size_t num_cols, resize_x_direction_e dir)
    {
        if(num_cols == 0)
            return;

        auto dv_x = div(num_cols, max_chunk_width);
        auto num_new_chunks_x = dv_x.quot;// + (dv_x.rem > 0)

        //create chunks if they didn't already exist
        if(chunks.size() == 0)
        {
            chunks.resize(std::min(num_new_chunks_x, 1));
        }

        // if new column will fit in existing chunks
        if(num_new_chunks_x == 0 && num_cols)
        {
            auto end_chunk = chunks[std::max(chunks.size() - 1), 0)];
            auto available_space = end_chunk.allocated_size - end_chunk.size;

            if(resize_x_direction_e == resize_rightwards
                && grow_amount.x <= available_space)
            {
                end_chunk.allocated_size.x += grow_amount.x;
            }
        }
        else //else add new chunks
        {
            auto prev_size = chunks.size();
            chunks.resize(chunks.size() + num_new_chunks_x); //add new col

            //add chunks in that col
            for(size_t x = prev_size; x < prev_size + num_new_chunks_x; ++x)
            {
                auto& col = chunks[i];

                // use same number of chunks as 0th column
                // if this is the 0th column, no rows will be added yet
                col.resize(chunks[0].size());

                //FIXME does not handle awkward sizes
                for(size_t y = 0; y < col.size(); ++y)
                {
                    chunk.position = ivec2(x, y);
                }
            }
        }
    }

    // WIP
    void add_rows(size_t num_rows, resize_y_direction_e)
    {
        if(num_rows == 0)
            return;

        //if no chunks exist, create a column to start
        
    }


    bool hex_grid_t::is_in_bounds(ivec2 hx) const
    {
        return hx.x >= 0 && hx.y >= 0 && hx.x < size.x && hx.y < size.y;
    }

    hex_grid_cell_t& hex_grid_t::cell_at(ivec2 hex_coord)
    {
        ASSERT(is_in_bounds(hex_coord), "Asking for a cell that is out of bounds");

        auto chunk_coord = chunk_coord_from_hex_coord(hex_coord);
        hex_grid_chunk_t& chunk = chunks[chunk_coord.x][chunk_coord.y];

        ivec2 local_hex_coord = hex_coord - (chunk_coord * ivec2(max_chunk_width, max_chunk_height));
        return chunk.cell_at_local_coord(local_hex_coord);
    }

    glm::ivec2 hex_grid_t::chunk_coord_from_hex_coord(ivec2 hex_coord) const
    {
        return hex_coord / ivec2(max_chunk_width, max_chunk_height);  //truncates
    }

    hex_grid_chunk_t& hex_grid_t::chunk_from_hex_coord(ivec2 hex_coord)
    {
        auto chunk_coord = chunk_coord_from_hex_coord(hex_coord);
        return chunks[chunk_coord.x][chunk_coord.y];
    }
}
}
}