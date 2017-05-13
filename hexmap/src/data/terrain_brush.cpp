#include "terrain_brush.h"

#include "asdf_multiplat/main/asdf_defs.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace hexmap
{
namespace data
{
    terrain_brush_t::terrain_brush_t(uvec2 size, bool init_val)
    {
        ASSERT(size.x > 0 && size.y > 0, "Invalid terrain brush size");

        brush_mask.resize(size.x * size.y, init_val);
    }

    terrain_brush_t::terrain_brush_t(terrain_brush_mask_t&& mask)
    : brush_mask(move(mask))
    {
    }


    terrain_brush_t terrain_brush_circle(float radius)
    {
        terrain_brush_mask_t mask;
        mask.resize(radius * 2 * radius * 2); //square with width/height equal to diameter

        for(size_t y = 0; y < radius * 2; ++y)
        {
            for(size_t x = 0; x < radius * 2; ++x)
            {
                auto v = glm::vec2(x,y) - glm::vec2(radius,radius);
                mask[x*(radius*2) + y] = (length(v) <= radius);
            }
        }

        return terrain_brush_t(move(mask));
    }

    terrain_brush_t terrain_brush_from_bitmap(std::string const& filepath)
    {
        EXPLODE("TODO");
        return terrain_brush_t();
    }

}
}
}