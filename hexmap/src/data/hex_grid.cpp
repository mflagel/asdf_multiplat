#include "hex_grid.h"


using namespace std;
using namespace glm;

namespace asdf
{
namespace hexmap
{
namespace data
{
    hex_grid_chunk_t::hex_grid_chunk_t(glm::ivec2 _size)
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
        // size_t num_chunks_x = std::ceil((double)size.x / (double)max_chunk_width);
        // size_t num_chunks_y = std::ceil((double)size.y / (double)max_chunk_height);

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
    }
}
}
}