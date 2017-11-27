#pragma once

#include <limits>
#include <cstdint>
#include <type_traits>

#include "asdf_dll.h"

#define ASDF_SDL2 1

#ifdef __llvm__
    #define LAX_MACROS 0
#else
    #define LAX_MACROS 1
#endif



#ifndef _MSC_VER
#define STRERROR(errno, buffer, buffsize) strerror_r(errno, buffer, buffsize); //UNIX
#define SNPRINTF(str, size, msg, ...) snprintf(str, size, msg, __VA_ARGS__);
#else
#define STRERROR(errno, buffer, buffsize) strerror_s(buffer, buffsize, errno); //MSVC
#define SNPRINTF(str, size, msg, ...) sprintf_s(str, size, msg, __VA_ARGS__);
#endif



#define PI 3.14159265359f


struct nullindex_t
{
    template<typename T>
    constexpr bool operator==(T const& rhs) const
    {
        return rhs == std::numeric_limits<T>::max();
    }
    template<typename T>
    constexpr bool operator!=(T const& rhs) const
    {
        return rhs != std::numeric_limits<T>::max();
    }
    template<typename T>
    constexpr operator T() const
    {
        return std::numeric_limits<T>::max();
    }
};

const nullindex_t nullindex = {};

template<typename T>
constexpr bool operator==(T const& lhs, nullindex_t const& rhs)
{
    return rhs == lhs;
}
template<typename T>
constexpr bool operator!=(T const& lhs, nullindex_t const& rhs)
{
    return rhs != lhs;
}

#define ASDF_UNUSED(x) (void)x


//LOGGING MACROS     ## before VA_ARGS required by clang/gcc
#define LOG(msg, ...) do{printf(msg, ##__VA_ARGS__); printf("\n");} while(0);
#define LOG_IF(condition, msg, ...) do{ if(condition){LOG(msg, ##__VA_ARGS__);} }while(0);

#define WARN_IF(condition, msg, ...) LOG_IF(condition, msg, ##__VA_ARGS__)



#define ASSERTS_ENABLED

#ifdef _MSC_VER
#define DEBUG_BREAK __debugbreak()
#else
//#include <signal.h>
#include <csignal>
#define DEBUG_BREAK std::raise(SIGTRAP);
#endif

//Assertion code shamelessly copymodified from: 
//http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/

//void asdf_fail(char const* condition, char const* file, int line, char const* msg, ...);
void asdf_fail(char const* condition, char const* file, int line, ...);

#ifdef ASSERTS_ENABLED  
    #define ASSERT(cond, ...) \
    do { \
        if (!(cond)) { \
            LOG("ASSERT FAILED: %s, %s, %i", #cond, __FILE__, __LINE__) \
            LOG(__VA_ARGS__); \
            DEBUG_BREAK; \
        } \
    } while(0)

    //like assert(false, ...)
    #define EXPLODE(...) \
        do \
        { \
            LOG("EXPLOSION: %s, %i", __FILE__, __LINE__); \
            LOG(__VA_ARGS__);\
            DEBUG_BREAK; \
        } while(0)
#else  
    #define ASSERT(condition) do { POW2_UNUSED(condition); } while (0)
    #define EXPLODE(message, ...) do { POW2_UNUSED(condition); } while (0)
#endif 




// Color Stuff
#ifdef glm_core_type
using color_t = glm::vec4;

#define COLOR_CLEAR     color_t(0.0f, 0.0f, 0.0f, 1.0f)
#define COLOR_WHITE     color_t(1.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_LIGHTGREY color_t(0.8f, 0.8f, 0.8f, 1.0f)
#define COLOR_LIGHTGRAY COLOR_LIGHTGREY
#define COLOR_GREY      color_t(0.5f, 0.5f, 0.5f, 1.0f)
#define COLOR_GRAY      COLOR_GREY
#define COLOR_DARKGREY  color_t(0.2f, 0.2f, 0.2f, 1.0f)
#define COLOR_DARKGRAY  COLOR_DARKGREY
#define COLOR_BLACK     color_t(0.0f, 0.0f, 0.0f, 1.0f)

#define COLOR_RED       color_t(1.0f, 0.0f, 0.0f, 1.0f)
#define COLOR_GREEN     color_t(0.0f, 1.0f, 0.0f, 1.0f)
#define COLOR_BLUE      color_t(0.0f, 0.0f, 1.0f, 1.0f)
#define COLOR_CYAN      color_t(0.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_YELLOW    color_t(1.0f, 1.0f, 0.0f, 1.0f)
#define COLOR_MAGENTA   color_t(1.0f, 0.0f, 1.0f, 1.0f)

#define COLOR_TEAL            color_t(0.0f, 1.0f, 0.5f, 1.0f)
#define COLOR_ORANGE          color_t(1.0f, 0.5f, 0.0f, 1.0f)
#define COLOR_CORNFLOWER_BLUE color_t(0.5f, 0.75f, 0.9f, 1.0f)

#endif


#define PRAGMA(x) _Pragma(#x)
#ifdef __clang__
#define DIAGNOSTIC_PUSH _Pragma("clang diagnostic push");
#define DIAGNOSTIC_POP _Pragma("clang diagnostic pop");
#define DIAGNOSTIC_IGNORE(to_ignore) PRAGMA(clang diagnostic ignored #to_ignore);
#define DIAGNOSTIC_ERROR(to_error) PRAGMA(clang diagnostic error #to_error);

// #elif __GNUC_
/// Apparently pragma diagnostics don't actually work in GCC when
/// compiling C++. This has been a bug for a long time...
/// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53431

// #define DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push");
// #define DIAGNOSTIC_POP _Pragma("GCC diagnostic pop");
// #define DIAGNOSTIC_IGNORE(to_ignore) PRAGMA(GCC diagnostic ignored #to_ignore);
// #define DIAGNOSTIC_ERROR(to_error) PRAGMA(GCC diagnostic error #to_error);

#else
#define DIAGNOSTIC_PUSH 
#define DIAGNOSTIC_POP
#define DIAGNOSTIC_IGNORE(to_ignore)
#define DIAGNOSTIC_ERROR(to_error)
#endif


#ifndef UINT32_MAX
#define UINT32_MAX = 4294967295U;
#endif


DIAGNOSTIC_PUSH
DIAGNOSTIC_IGNORE(-Wsign-compare)

template <typename A, typename B>
B convert_integer(A value)
{
    /// FIXME gives error / warning in GCC
    // ASSERT(value <= std::numeric_limits<B>::max(), "value is too large to convert without overflow");
    // ASSERT(value >= std::numeric_limits<B>::min(), "value is too small to convert without underflow");

    return static_cast<B>(value);
}

DIAGNOSTIC_POP


inline int32_t unsigned_to_signed(uint32_t u)
{
    return convert_integer<uint32_t,int32_t>(u);
}

inline uint32_t signed_to_unsigned(int32_t i)
{
    return convert_integer<int32_t,uint32_t>(i);
}


/// https://stackoverflow.com/a/24018996
/// Super interesting. std::conditional basically works such that
/// if T and U are the same, it 'returns' true, but if false, 
/// recursively calls is_any_of with the rest of the args
template<typename T, typename U, typename... Us>
struct is_any_of
    : std::integral_constant<
        bool,
        std::conditional<
            std::is_same<T,U>::value,
            std::true_type,
            is_any_of<T,Us...>
        >::type::value
      >
{ };

template<typename T, typename U>
struct is_any_of<T,U> : std::is_same<T,U>::type { };
///


/// https://stackoverflow.com/a/24855290
// template<class T>
// struct is_c_str
//   : std::integral_constant<
//       bool,
//       std::is_same<char const *, typename std::decay<T>::type>::value ||
//       std::is_same<char *, typename std::decay<T>::type>::value
// > {};
template<class T>
struct is_c_str
  : std::integral_constant<
      bool,
      std::is_same<char *, typename std::remove_reference<typename std::remove_cv<T>::type>::type>::value ||
      std::is_same<char const *, typename std::remove_reference<typename std::remove_cv<T>::type>::type>::value
> {};
///


/// https://stackoverflow.com/a/21510185
/// allows a range-based for loop to access a container in reverse
/// ex:
///     for (auto x: reversed(c))
///         ...
namespace details {
    template <class T> struct _reversed { T& t; _reversed(T& _t): t(_t) {} };
    template <class T> struct _creversed { T const& t; _creversed(T const& _t): t(_t) {} };  /// ADDED by mflagel
}

template <class T> details::_reversed<T> reversed(T& t) { return details::_reversed<T>(t); }
template <class T> details::_reversed<T const> reversed(T const& t) { return details::_reversed<T const>(t); }

namespace std {
    template <class T> auto begin(details::_reversed<T>& r) -> decltype(r.t.rbegin()) { return r.t.rbegin(); }
    template <class T> auto end(details::_reversed<T>& r) -> decltype(r.t.rend()) { return r.t.rend(); }

    template <class T> auto begin(details::_creversed<T> const& cr) -> decltype(cr.t.rbegin()) { return cr.t.rbegin(); }
    template <class T> auto end(details::_creversed<T> const& cr) -> decltype(cr.t.rend()) { return cr.t.rend(); }
}
///


/// Delete / Default operators

#define DEFAULT_COPY_ASSIGNMENT(_obj_name_) \
    _obj_name_(_obj_name_ const&) = default;  \
    _obj_name_& operator=(_obj_name_ const&) = default;
/*--------*/

#define DELETE_COPY_ASSIGNMENT(_obj_name_) \
    _obj_name_(_obj_name_ const&) = delete;  \
    _obj_name_& operator=(_obj_name_ const&) = delete;
/*--------*/


#define DEFAULT_MOVE(_obj_name_) \
    _obj_name_(_obj_name_&&) = default; \
    _obj_name_& operator=(_obj_name_&&) = default;
/*--------*/

#define DELETE_MOVE(_obj_name_) \
    _obj_name_(_obj_name_&&) = delete; \
    _obj_name_& operator=(_obj_name_&&) = delete;
/*--------*/


#define DELETE_COPY_ASSIGNMENT_MOVE(_obj_name_) \
    _obj_name_(const _obj_name_&) = delete; \
    _obj_name_& operator=(_obj_name_ const&) = delete; \
    _obj_name_(_obj_name_&&) = delete; \
    _obj_name_& operator=(_obj_name_&&) = delete;
/*--------*/


#define UNIQUE_OBJECT_ASSIGN_COPY_MOVE(_obj_name_) \
    DELETE_COPY_ASSIGNMENT(_obj_name_) \
    DEFAULT_MOVE(_obj_name_) 
/*--------*/
