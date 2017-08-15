#pragma once

#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <variant>


#ifdef _MSC_VER
#include <filesystem>
#else
#include <experimental/filesystem>
#endif


#include <asdfm/main/asdf_defs.h>


namespace stdfs = std::experimental::filesystem;

namespace plantgen
{
    
    struct multi_value_t
    {
        size_t num_to_pick;

        std::vector<std::string> values;
    };
    using range_value_t = std::vector<std::string>;
    
    /// I'd rather use nullptr_t for the null type, but the compiler
    /// complains at the operator<< overload that takes a variant_value_t
    using null_value_t = int;

    /// null_value_t is the first variant subtype, and thus the one that is used for the default ctor
    /// which sort of makes it an optional<variant<...>>
    using variant_value_t = std::variant<null_value_t, std::string, range_value_t, multi_value_t>;
    /// user values are data that isn't a property and isnt a rollable value
    /// useful for programs that extend plant_generator
    using user_value_t = std::variant<null_value_t, bool, double, std::string>; //ie: json value types

    // This is where C++ gets kind of weird....  SFINAE
    // enable_if T is not a variant_value
    // (otherwise the compiler complains that this overlaps with the std default variant-vs-variant comparator)

    template <class T>
    struct is_variant_subtype : is_any_of<T
                                        , std::string
                                        , range_value_t
                                        , multi_value_t
                                        , null_value_t>
                                        {};

    template <typename T>
    struct is_variant_value_comparable : std::integral_constant<bool,
                                           is_variant_subtype<T>::value
                                        || std::is_same_v<char*, typename std::remove_cv<T>>
                                        || std::is_same_v<char const*, typename std::remove_reference<T>> >
                                        {};


    /// Helper Aliases
    template <typename T>
    inline constexpr bool is_variant_subtype_v = is_variant_subtype<T>::value;
    template <typename T>
    inline constexpr bool is_variant_value_comparable_v = is_variant_value_comparable<T>::value;


    template <typename T,
        typename = std::enable_if_t<is_variant_value_comparable_v<T>> >
    bool operator==(variant_value_t const& v, T const& t)
    {
        return std::get<T>(v) == t;
    }
    template <typename T,
        typename = std::enable_if_t<is_variant_value_comparable_v<T>> >
    bool operator==(T const& t, variant_value_t const& v)
    {
        return std::get<T>(v) == t;
    }
    
    template <typename T,
        typename = std::enable_if_t<is_variant_value_comparable_v<T>> >
    bool operator!=(variant_value_t const& v, T const& t)
    {
        return std::get<T>(v) != t;
    }
    template <typename T,
        typename = std::enable_if_t<is_variant_value_comparable_v<T>> >
    bool operator!=(T const& t, variant_value_t const& v)
    {
        return std::get<T>(v) != t;
    }


    inline bool operator==(variant_value_t const& v, const char* const c)
    {
        return strcmp(std::get<std::string>(v).c_str(), c) == 0;
    }
    inline bool operator==(const char* const c, variant_value_t const& v)
    {
        return v == c;
    }
    inline bool operator!=(variant_value_t const& v, const char* const c)
    {
        return !(v == c);
    }
    inline bool operator!=(const char* const c, variant_value_t const& v)
    {
        return !(v == c);
    }


    /// Required to prevent an ambiguous overload with the above comparison funcs
    inline bool operator!=(multi_value_t const& lhs, multi_value_t const& rhs)
    {
        return lhs.num_to_pick != rhs.num_to_pick
            && lhs.values != rhs.values;
    }
    inline bool operator==(stdfs::path path, const char* const c)
    {
        return strcmp(path.c_str(), c) == 0;
    }
    ///
    

    // The weight value is not a percentage, but rather
    // a multiplier. A value is 'weight' times more likely
    // to be picked (as if it was entered into the value list
    // 'weight' times).
    struct weighted_value_t : public variant_value_t
    {
        using variant_value_t::variant_value_t;

        uint32_t weight = 1;
    };

    // using value_list_t = std::vector<weighted_value_t>;


    template <typename T,
        typename = std::enable_if_t<is_variant_value_comparable_v<T>> >
    bool operator==(weighted_value_t const& wv, T const& v)
    {
        return static_cast<variant_value_t>(wv) == v;
    }
    template <typename T,
        typename = std::enable_if_t<is_variant_value_comparable_v<T>> >
    bool operator==(T const& v, weighted_value_t const& wv)
    {
        return wv == v;
    }
    
    template <typename T,
        typename = std::enable_if_t<is_variant_value_comparable_v<T>> >
    bool operator!=(weighted_value_t const& wv, T const& v)
    {
        return !(wv == v);
    }
    template <typename T,
        typename = std::enable_if_t<is_variant_value_comparable_v<T>> >
    bool operator!=(T const& v, weighted_value_t const& wv)
    {
        return wv != v;
    }

    inline bool operator!=(weighted_value_t const& wv, const char* const c)
    {
        return static_cast<variant_value_t>(wv) != c;
    }



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




    // template <typename L>
    // bool operator==(value_list_t const& value_list, L const& comp_list)
    // {
    //     if(value_list.size() != comp_list.size())
    //         return false;

    //     /*for(size_t i = 0; i < value_list.size(); ++i)
    //     {
    //         if(value_list[i] != comp_list[i])
    //             return false;
    //     }*/

    //     // use range based for so that this function can take an
    //     // initializer list which has no operator[] but has
    //     // begin/end iterators
    //     size_t i = 0;
    //     for(auto const& c : comp_list)
    //     {
    //         if(value_list[i] != c)
    //             return false;

    //         ++i;
    //     }

    //     return true;
    // }
    // template <typename L>
    // bool operator==(L const& comp_list, value_list_t const& value_list)
    // {
    //     return value_list == comp_list;
    // }

    // template <typename L>
    // bool operator!=(value_list_t const& value_list, L const& comp_list)
    // {
    //     return !(value_list == comp_list);
    // }
    // template <typename L>
    // bool operator!=(L const& comp_list, value_list_t const& value_list)
    // {
    //     return !(value_list == comp_list);
    // }

}