#include "spline.h"

namespace asdf {
namespace hexmap {
namespace data
{

    void spline_selection_t::select_spline(spline_t& spline)
    {
        std::vector<size_t> new_inds;
        new_inds.reserve(spline.size());
        for(size_t i = 0; i < spline.size(); ++i)
        {
            new_inds.push_back(i);
        }

        select_spline_nodes(spline, std::move(new_inds));
    }

    void spline_selection_t::select_spline_nodes(spline_t& spline, std::vector<size_t> node_inds)
    {
        if(is_control_node)
        {
            deselect_all();
            is_control_node = false;
        }

        splines.push_back(&spline);
        node_indices.push_back(std::move(node_inds));
    }

    void spline_selection_t::select_control_node(spline_t& spline, size_t control_node_index)
    {
        deselect_all();
        is_control_node = true;

        splines.push_back(&spline);

        node_indices.resize(1);
        node_indices[0].push_back(control_node_index);
    }


    void spline_selection_t::deselect_all()
    {
        splines.clear();
        node_indices.clear();
    }

}
}
}
