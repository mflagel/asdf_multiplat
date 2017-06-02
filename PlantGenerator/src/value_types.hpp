#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace plantgen
{
    
    struct multi_value_t
    {
        size_t num_to_pick;

        std::vector<std::string> values;
    };
    using range_value_t = std::vector<std::string>;
    using null_value_t = int;
    
    using variant_value_t = std::variant<std::string, range_value_t, multi_value_t, null_value_t>;

    // This is where C++ gets kind of weird....  SFINAE
    // enable_if T is not a variant_value
    // (otherwise the compiler complains that this overlaps with the std default variant-vs-variant comparator)
    template< class T>
    struct is_variant_subtype : std::integral_constant<bool,
                                  std::is_same<T, std::string>::value
                               || std::is_same<T, range_value_t>::value
                               || std::is_same<T, multi_value_t>::value
                               || std::is_same<T, null_value_t>::value>
                               {};


    template <typename T, typename = std::enable_if_t<is_variant_subtype<T>::value> >
    bool operator==(variant_value_t const& v, T const& t)
    {
        return std::get<T>(v) == t;
    }
    template <typename T, typename = std::enable_if_t<is_variant_subtype<T>::value> >
    bool operator==(T const& t, variant_value_t const& v)
    {
        return std::get<T>(v) == t;
    }
    
    template <typename T, typename = std::enable_if_t<is_variant_subtype<T>::value> >
    bool operator!=(variant_value_t const& v, T const& t)
    {
        return std::get<T>(v) != t;
    }
    
    /* Compiler complains about having two overloads for this function
    template <typename T, typename = std::enable_if_t<is_variant_subtype<T>::value && !std::is_void<T>::value>>
    bool operator!=(T const& t, variant_value_t const& v)
    {
        return std::get<T>(v) != t;
    }
    */
    
    

    // The weight value is not a percentage, but rather
    // a multiplier. A value is 'weight' times more likely
    // to be picked (as if it was entered into the value list
    // 'weight' times).
    struct weighted_value_t : public variant_value_t
    {
        using variant_value_t::variant_value_t;

        uint32_t weight = 1;
    };

    using value_list_t = std::vector<weighted_value_t>;

    template <typename L>
    bool operator==(value_list_t const& value_list, L const& comp_list)
    {
        if(value_list.size() != comp_list.size())
            return false;

        for(size_t i = 0; i < value_list.size(); ++i)
        {
            if(value_list[i] != comp_list[i])
                return false;
        }

        return true;
    }
    template <typename L>
    bool operator==(L const& comp_list, value_list_t const& value_list)
    {
        return value_list == comp_list;
    }

    template <typename L>
    bool operator!=(value_list_t const& value_list, L const& comp_list)
    {
        return !(value_list == comp_list);
    }
}