#include "stdafx.h"
#include "hex_grid.h"

//#include "to_from_json.h"

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

    hex_grid_t::hex_grid_t(std::string const& filepath)
    {
        load_from_file(filepath);
    }


    void hex_grid_t::load_from_file(std::string const& filepath)
    {
        SDL_RWops* io = SDL_RWFromFile(filepath.c_str(), "rb");

        if (!io)
        {
            EXPLODE("hex_grid_t::load_to_file()");  //todo: handle errors better
        }


        hxm_header_t header;
        size_t num_read = SDL_RWread(io, &header, sizeof (hxm_header_t), 1);
        ASSERT(num_read > 0, "Error reading file header");


        init(header.map_size, header.chunk_size);

        for_each_chunk([&](hex_grid_chunk_t& chunk)
        {
            SDL_RWread(io, &(chunk.size), sizeof(glm::uvec2), 1);

            for(auto& column : chunk.cells)
            {
                size_t num_r = SDL_RWread(io, column.data(), sizeof(hex_grid_cell_t), column.size());
                ASSERT(num_r == column.size(), "");
                LOG("read %d cells", num_r);
            }
        });


        SDL_RWclose(io);
    }


    // https://wiki.libsdl.org/SDL_RWops?highlight=%28%5CbCategoryStruct%5Cb%29%7C%28CategoryIO%29
    // https://wiki.libsdl.org/SDL_RWwrite?highlight=%28%5CbCategoryIO%5Cb%29%7C%28CategoryEnum%29%7C%28CategoryStruct%29
    void hex_grid_t::save_to_file(std::string const& filepath)
    {
        WARN_IF(!chunks.empty(), "Saving an empty map");

        SDL_RWops* io = SDL_RWFromFile(filepath.c_str(), "wb");

        if(!io)
        {
            EXPLODE("hex_grid_t::save_to_file() error");  //todo: handle errors better
        }

        hxm_header_t header;
        header.version = 0;
        header.chunk_size = chunk_size();
        header.map_size = size;

        size_t num_written = SDL_RWwrite(io, (void*)&header, sizeof(hxm_header_t), 1);

        if(num_written != 1)
        {
            EXPLODE("error writing hxm header");
        }


        for_each_chunk([&](hex_grid_chunk_t const& chunk)
        {
            SDL_RWwrite(io, (void*)&chunk.size, sizeof(glm::uvec2), 1);
            for(auto column : chunk.cells)
            {
                size_t num_w = SDL_RWwrite(io, column.data(), sizeof(hex_grid_cell_t), column.size());
                ASSERT(num_w == column.size(), "chunk save error");
                LOG("wrote %d cells", num_w);
            }
        });
        

        SDL_RWclose(io);
    }


    hex_grid_t::hex_grid_t(glm::ivec2 _size)
    : size(_size)
    {
        init(size);
    }


    void hex_grid_t::init(glm::ivec2 _size, glm::uvec2 chunk_size)
    {
        auto dv_x = div(size.x, chunk_size.x);
        auto dv_y = div(size.y, chunk_size.y);

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
    }

    bool hex_grid_t::is_in_bounds(ivec2 hx) const
    {
        return hx.x >= 0 && hx.y >= 0 && hx.x < size.x && hx.y < size.y;
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


    glm::ivec2 world_to_hex_coord(glm::vec2 world_pos)
    {
        //adjust so that world 0,0 is the bottom left of hexagon 0,0
        world_pos += vec2(hex_width_d2, hex_height_d2);

        //convert mouse world coords to a hexagon coord
        float sub_column = world_pos.x / hex_width_d4;
        float sub_row    = world_pos.y / hex_height_d2;

        //LOG("subcol: %f   subrow: %f", sub_column, sub_row);

        int column = static_cast<int>(glm::floor(sub_column / 3.0f));
        int row    = static_cast<int>(glm::floor(world_pos.y / hex_height));

        //if column is within hex_width_d4 of the column center (ie: fraction is +- 0.25) then it's only one column
        if(static_cast<int>(glm::floor(sub_column)) % 3 == 0) //horizontal overlap every 3 sub-columns (with a width of one sub-column)
        {
            //todo: handle column overlap

            bool even = static_cast<int>(std::abs(floor(sub_row))) % 2 == 0;
            //even rows slant right  '/'
            //odd rows slant left    '\'

            vec2 line;

            auto angle = (even * 1.0f + PI) / 3.0f;
            line.x = cos(angle);
            line.y = sin(angle);
            line *= hex_edge_length;

            vec2 p0(floor(sub_column) * hex_width_d4, floor(sub_row) * hex_height_d2); //bottom point of slant

            auto p1 = p0 + line;
            auto const& p2 = world_pos;
            auto side = (p1.x - p0.x)*(p2.y - p0.y) - (p1.y - p0.y)*(p2.x - p0.x);  //FIXME
            //((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x))
            
            
            //LOG("side: %f", side);
            column -= 1 * (side < 0);
        }

        //if odd column, adjust row down
        if(column % 2 == 1)
        {
            row = static_cast<int>(glm::floor((world_pos.y + hex_height_d2) / hex_height));
        }

        return ivec2(column, row);
    }

}
}