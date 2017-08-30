#pragma once

#define PI 3.14159265359f
#define nullindex 4294967295

#define ASDF_UNUSED(x) (void)x


//LOGGING MACROS     ## before VA_ARGS required by clang/gcc
#define LOG(msg, ...) do{printf(msg, ##__VA_ARGS__); printf("\n");} while(0);
#define LOG_IF(condition, msg, ...) do{ if(condition){LOG(msg, ##__VA_ARGS__);} }while(0);

#define WARN_IF(condition, msg, ...) LOG_IF(condition, msg, ##__VA_ARGS__)



#define ASSERTS_ENABLED

#ifdef _MSC_VER
#define DEBUG_BREAK __debugbreak()
#else
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP);
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


// #include <type_traits>

DIAGNOSTIC_PUSH
DIAGNOSTIC_IGNORE(-Wsign-compare)

template <typename A, typename B>
B convert_integer(A value)
{
    ASSERT(value <= std::numeric_limits<B>::max(), "value is too large to convert without overflow");
    ASSERT(value >= std::numeric_limits<B>::min(), "value is too small to convert without underflow");

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

>>>>>>> hexmap_qt
