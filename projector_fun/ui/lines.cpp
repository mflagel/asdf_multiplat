#include "stdafx.h"
#include "lines.h"

using namespace std;
using namespace glm;

namespace asdf
{
namespace projector_fun
{

    /* Technique from https://www.mapbox.com/blog/drawing-antialiased-lines/
        Each line segment consists of a quad of variable thickness
        This quad is built with a thickness of zero (such that verts 0 and 1 have equal positions, as to verts 2,3)
        Each vert has a normal, which the vertex shader will use to translate the vertex to expand the quad to the desired thickness
    */
    std::vector<line_vertex_t> build_verts()
    {
        ASSERT(line_points.size() >= 2, "Not much of a line with less than two points");

        std::vector<line_vertex_t> verts;
        verts.reserve(line_points.size() * 2);

        verts.push_back(build_verts(verts, line_points[0], line_points[1]));

        for(size_t i = 1; i + 1 < line_points.size(); ++i)
        {
            verts.push_back(build_verts(verts, line_points[i], line_points[i - 1]));
            // verts.push_back(build_verts(verts, line_points[i - 1], line_points[i], line_points[i - 1]));
        }

        verts.push_back(build_verts(verts, line_points[i].back(), line_points[line_points.size()-2]));
        //flip normals since we threw points in backwards
        verts[verts.size()-1].normal *= -1;
        verts[verts.size()-2].normal *= -1;

        return verts;
    }

    /*
        Adds the vertices meant for p0 to verts, with normals built for the line segment {p0, p1}
        Points are meant to be rendered as a triangle strip
        TODO: support 3d

       /|\           /|\
      0 #-------------# 2
        |       ,.-'` |
        | ,.-'`       |
      1 #-------------# 3
       \|/           \|/

       This diagram represents what it will look like after the points are moved along
       their normal in the vertex shader.

       Only points 0 and 1 are
    */
    void build_verts(vertices_t& verts, line_point_t const& p0, line_point_t const& p1)
    {
        vec3 vec = p1.pos - p0.pos;
        vec = normalize(vec);

        verts.push_back(line_vertex_t{p0.pos, glm::vec3{ vec.x, -vec.y, vec.0});
        verts.push_back(line_vertex_t{p0.pos, glm::vec3{-vec.x,  vec.y, vec.0});
    }

    void build_verts(vertices_t& verts, line_point_t const&, line_point_t const&, line_point_t const&)
    {

    }

}
}