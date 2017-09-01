#include "value_types.hpp"
#include "plant_generator.h"

namespace plantgen
{
    // empty dtor defined here so that I can use a unique ptr
    multi_value_t::~multi_value_t()
    {}

    multi_value_t::multi_value_t(multi_value_t const& mv)
    : num_to_pick(mv.num_to_pick)
    {
        //deep copy nodes
        for(auto const& v : mv.values)
        {
            pregen_node_t const& node = *v;
            values.push_back(std::make_unique<pregen_node_t>(node));
        }
    }

    multi_value_t& multi_value_t::operator=(multi_value_t const& mv)
    {
        num_to_pick = mv.num_to_pick;

        //deep copy nodes
        values.clear();
        for(auto const& v : mv.values)
        {
            pregen_node_t const& node = *v;
            values.push_back(std::make_unique<pregen_node_t>(node));
        }

        return *this;
    }


    
    std::ostream& operator<<(std::ostream& os, multi_value_t const& obj)
    {
        std::string s = std::to_string(obj.num_to_pick) + " from {";

        /*for(auto const& val : obj.values)
            s += val + ", ";
            */

        /// TODO: prettier printing when all value nodes contain strings
        for(auto const& vn : obj.values)
            s += to_string(*vn);

        s.resize(s.size() - 1);
        s.back() = '}';

        os << s;
        return os;
    }
}