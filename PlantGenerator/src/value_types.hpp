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

    using user_value_t = std::variant<bool, double, std::string>; //ie: random json data
    using user_data_t = std::unordered_map<std::string, user_value_t>;

    // This is where C++ gets kind of weird....  SFINAE
    // enable_if T is not a variant_value
    // (otherwise the compiler complains that this overlaps with the std default variant-vs-variant comparator)
    template< class T>
    struct is_variant_subtype : std::integral_constant<bool,
                                  std::is_same<T, std::string>::value
                               || std::is_same<T, range_value_t>::value
                               || std::is_same<T, multi_value_t>::value
                               || std::is_same<T, null_value_t>::value >
                               {};

    template <typename T>
    struct is_variant_value_comparable : std::integral_constant<bool,
                                           is_variant_subtype<T>::value
                                        || std::is_same<std::remove_const<T>, char*>::value >
                                        {};


    template <typename T, typename = std::enable_if_t<is_variant_value_comparable<std::decay<T>>::value> >
    bool operator==(variant_value_t const& v, T const& t)
    {
        return std::get<T>(v) == t;
    }
    template <typename T, typename = std::enable_if_t<is_variant_value_comparable<std::decay<T>>::value> >
    bool operator==(T const& t, variant_value_t const& v)
    {
        return std::get<T>(v) == t;
    }
    
    template <typename T, typename = std::enable_if_t<is_variant_value_comparable<std::decay<T>>::value> >
    bool operator!=(variant_value_t const& v, T const& t)
    {
        return std::get<T>(v) != t;
    }
    
    /* Compiler complains about having two overloads for this function
    template <typename T, typename = std::enable_if_t<is_variant_value_comparable<T>::value && !std::is_void<T>::value>>
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



    /// SFINAE is terribly obnoxious
    /*template <typename T, typename = std::enable_if_t<std::is_same<std::decay<T>, variant_value_t>::value
                                                   || std::is_same<std::decay<T>, user_value_t>::value >>
    std::ostream& operator<<(std::ostream& os, T const& obj)
    {
        std::visit( [&os](auto const& arg)
            {
                os << arg;
            }
        , obj);
        
        return os;
    }*/

    inline std::ostream& operator<<(std::ostream& os, range_value_t const& obj)
    {
        std::string s = "{";
        for(auto const& val : obj)
            s += val + ", ";

        s.resize(s.size() - 1);
        s.back() = '}';

        os << s;
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, multi_value_t const& obj)
    {
        std::string s = std::to_string(obj.num_to_pick) + " from {";

        for(auto const& val : obj.values)
            s += val + ", ";

        s.resize(s.size() - 1);
        s.back() = '}';

        os << s;
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, variant_value_t const& obj)
    {
        std::visit( [&os](auto const& arg)
            {
                os << arg;
            }
        , obj);
        
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, weighted_value_t const& obj)
    {
        if(obj.weight > 1)
            os << "%" << obj.weight << " ";

        os << static_cast<variant_value_t const&>(obj);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, user_value_t const& obj)
    {
        std::visit( [&os](auto const& arg)
            {
                os << arg;
            }
        , obj);
        
        return os;
    }





    template <typename L>
    bool operator==(value_list_t const& value_list, L const& comp_list)
    {
        if(value_list.size() != comp_list.size())
            return false;

        /*for(size_t i = 0; i < value_list.size(); ++i)
        {
            if(value_list[i] != comp_list[i])
                return false;
        }*/

        // use range based for so that this function can take an
        // initializer list which has no operator[] but has
        // begin/end iterators
        size_t i = 0;
        for(auto const& c : comp_list)
        {
            if(value_list[i] != c)
                return false;

            ++i;
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