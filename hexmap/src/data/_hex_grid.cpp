#include "stdafx.h"
#include "hex_grid.h"
#include "data/hex_util.h"
#include "ui/hex_map.h"

//#include "to_from_json.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace hexmap
{
namespace data
{
    hex_grid_chunk_t::hex_grid_chunk_t(glm::uvec2 _size, hex_grid_cell_t const& default_cell_style)
    : size(_size)
    , allocated_size(_size)
    {
        cells.resize(size.x);

        for(auto& grid_y : cells)
        {
            grid_y.resize(size.y, default_cell_style);
        }
    }

    bool hex_grid_chunk_t::operator==(hex_grid_chunk_t const& rhs) const
    {

        return position       == rhs.position
            && size           == rhs.size
            && allocated_size == rhs.allocated_size
            && contents_equal(rhs);
    }

    bool hex_grid_chunk_t::contents_equal(hex_grid_chunk_t const& rhs) const
    {
        if(size != rhs.size)
            return false;

        for(size_t y = 0; y < size.y; ++y)
        {
            for(size_t x = 0; x < size.x; ++x)
            {
                if(cells[x][y] != rhs.cells[x][y])
                    return false;
            }
        }

        return true;
    }


    /* --Save Format--
        For each chunk
            write the size of the chunk (two unsigned 32-bit integers)
            for each column in the chunk
                write the cells of this column
    */
    void hex_grid_t::save_to_file(SDL_RWops* io)
    {
        ASSERT(io, "can't save a hex_grid to a null SDL_RWops");
        WARN_IF(chunks.empty(), "Saving an empty hex_grid");

        size_t total_bytes = 0;

        for_each_chunk([&](hex_grid_chunk_t const& chunk)
        {
            SDL_RWwrite(io, reinterpret_cast<const void*>(&chunk.size), sizeof(glm::uvec2), 1);
            for(auto column : chunk.cells)
            {
                size_t num_w = SDL_RWwrite(io, column.data(), sizeof(hex_grid_cell_t), column.size());
                ASSERT(num_w == column.size(), "chunk save error");
                LOG("wrote %zu cells", num_w);

                total_bytes += num_w * sizeof(hex_grid_cell_t);
            }
        });

        LOG("total hex_grid bytes written: %zu", total_bytes);
    }

    void hex_grid_t::load_from_file(SDL_RWops* io, hxm_header_t const& header)
    {
        chunks.clear();
        init(header.map_size, header.chunk_size);

        size_t total_bytes = 0;

        for_each_chunk([&](hex_grid_chunk_t& chunk)
        {
            SDL_RWread(io, &(chunk.size), sizeof(glm::uvec2), 1);

            for(auto& column : chunk.cells)
            {
                size_t num_r = SDL_RWread(io, column.data(), sizeof(hex_grid_cell_t), column.size());
                ASSERT(num_r == column.size(), "");
                LOG("read %zu cells", num_r);

                total_bytes += num_r * sizeof(hex_grid_cell_t);
            }
        });

        LOG("total hex_grid bytes read: %zu", total_bytes);
    }


    hex_grid_t::hex_grid_t(glm::uvec2 _size, hex_grid_cell_t const& default_cell_style)
    : size(_size)
    {
        init(size, default_cell_style);
    }

    void hex_grid_t::init(glm::uvec2 _size, glm::uvec2 chunk_size, hex_grid_cell_t const& default_cell_style)
    {
        size = _size;

        auto dv_x = div(long(_size.x), chunk_size.x); //cast to long since there's no overload of div that takes a unsigned int
        auto dv_y = div(long(_size.y), chunk_size.y); //

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
                auto& chunk = chunks[x][y];

                //init cells
                for(auto& column : chunk.cells)
                    for(auto& cell : column)
                        cell = default_cell_style;

                //set position to be used when rendering (acts like an {x,y} offset)
                chunk.position = ivec2(x,y);

                // adjust the size of the furthest chunks in this **column** if necessary
                // allows for map sizes that aren't multiples of chunk_size
                if(dv_x.rem > 0 && x == num_chunks_x - 1)
                    chunk.size.x = dv_x.rem;
            }

            // adjust the size of the furthest chunks in this **row** if necessary
            // allows for map sizes that aren't multiples of chunk_size
            if(dv_y.rem > 0)
                chunks[x][chunks[x].size() - 1].size.y = dv_y.rem;
        }
    }

    void hex_grid_t::init(glm::uvec2 size, hex_grid_cell_t const& default_cell_style)
    {
        init(size, glm::uvec2(new_chunk_width, new_chunk_height), default_cell_style);
    }

    /// Resize by allocating an entirely new hex-grid and copying the data over
    void resize_by_copy(glm::ivec2 new_size, resize_x_direction_e, resize_y_direction_e)
    {
        //TODO: create new array of chunks with new size and copy data
        // sounds expensive as hell
    }

    void hex_grid_t::resize(glm::uvec2 new_size, resize_x_direction_e xdir, resize_y_direction_e ydir)
    {
        using lvec2 = glm::detail::tvec2<int64_t>;

        lvec2 l_size(size.x, size.y);
        lvec2 l_newsize(new_size.x, new_size.y);

        lvec2 diff = l_newsize - l_size;
        bool grow_x = diff.x > 0;
        bool grow_y = diff.y > 0;
        bool shrink_x = diff.x < 0;
        bool shrink_y = diff.y < 0;

        // if(grow_x && grow_y)  //grow both
        // {
        //     grow(new_size - size, xdir, ydir);
        // }
        // else if(shrink_x && shrink_y)  //shrink both
        // {
        //     shrink(new_size - size, xdir, ydir);
        // }

        //shrink before grow to minimize data storage needed to undo such an action
        if(shrink_x)
        {
            shrink( uvec2(abs(diff.x), 0), xdir, ydir );
        }
        if(shrink_y)
        {
            shrink( uvec2(0, abs(diff.y)), xdir, ydir );
        }
        if(grow_x)
        {
            grow( uvec2(diff.x, 0), xdir, ydir );
        }
        if(grow_y)
        {
            grow( uvec2(0, diff.y), xdir, ydir );
        }
    }

    void hex_grid_t::grow(glm::uvec2 grow_amount, resize_x_direction_e xdir, resize_y_direction_e ydir)
    {
        ASSERT(grow_amount.x >=0 && grow_amount.y >= 0, "");

        //     add_columns(grow_amount.x, xdir);
        //     add_rows(grow_amount.y, ydir);
    }

    void hex_grid_t::shrink(glm::uvec2 shrink_amount, resize_x_direction_e, resize_y_direction_e)
    {
        ASSERT(shrink_amount.x >=0 && shrink_amount.y >= 0, "");
        EXPLODE("todo: hex_grid_t::shrink()");
    }


/*
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
    */


    //FIXME handle cases where columns or rows have been added below 0,0
    bool hex_grid_t::is_in_bounds(ivec2 hx) const
    {
        return hx.x >= 0 && hx.y >= 0 && hx.x < size.x && hx.y < size.y;
    }

    bbox_units_t hex_grid_t::bounding_box_units() const
    {
        if(chunks.empty())
            return bbox_units_t{vec2(0), vec2(0)};

        //start with the bounds of cell positions
        bbox_units_t bb;
        bb.lower = chunks[0][0].position * ivec2(chunk_size());
        bb.upper = chunks.back().back().position * ivec2(chunk_size());
        bb.upper.x += chunks.back().front().size.x;
        bb.upper.y += chunks.back().back().size.y;

        //scale vertically since hexes aren't one unit tall
        bb.lower.y *= hex_height;
        bb.upper.y *= hex_height;

        //padding to account for column height offset and overlap (if applicable)
        if(chunks.size() > 1) ///FIXME handle vertical hexes
        {
            glm::vec2 size = bb.upper - bb.lower;
            bb.upper.x -= size.x * hex_width_d4;  //horizontal overlap
            bb.lower.y -= hex_height_d2; //vertical offset
        }

        //add padding to account for the fact that cell positions are at their centers
        bb.lower -= vec2(hex_width_d2, hex_height_d2);
        bb.upper += vec2(hex_width_d2, hex_height_d2);

        return bb;
        
    }

    glm::vec2 hex_grid_t::size_units() const
    {
        auto bbox = bounding_box_units();
        return vec2(bbox.upper - bbox.lower);
    }

    glm::uvec2 hex_grid_t::size_chunks() const
    {
        if(chunks.empty())
            return glm::uvec2(0);

        glm::uvec2 size;
        size.x = chunks.size();
        size.y = chunks[0].size();

        return size;
    }

    glm::uvec2 hex_grid_t::chunk_size() const
    {
        if(chunks.empty())
        {
            return glm::uvec2(new_chunk_width, new_chunk_height);
        }
        else
        {
            return chunks[0][0].allocated_size;
        }
    }

    hex_grid_cell_t& hex_grid_t::cell_at(ivec2 hex_coord)
    {
        ASSERT(is_in_bounds(hex_coord), "Asking for a cell that is out of bounds");

        auto chunk_coord = chunk_coord_from_hex_coord(hex_coord);
        hex_grid_chunk_t& chunk = chunks[chunk_coord.x][chunk_coord.y];

        ivec2 local_hex_coord = hex_coord - (chunk_coord * ivec2(new_chunk_width, new_chunk_height));
        return chunk.cell_at_local_coord(local_hex_coord);
    }

    // hex_grid_cell_t& hex_grid_t::cell_at(glm::vec2 world_pos)
    // {
    // }

    glm::ivec2 hex_grid_t::chunk_coord_from_hex_coord(ivec2 hex_coord) const
    {
        return hex_coord / ivec2(new_chunk_width, new_chunk_height);  //truncates
    }

    hex_grid_chunk_t& hex_grid_t::chunk_from_hex_coord(ivec2 hex_coord)
    {
        auto chunk_coord = chunk_coord_from_hex_coord(hex_coord);
        return chunks[chunk_coord.x][chunk_coord.y];
    }

}
}
}