// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


//std includes
#include <stdio.h>
// #include <tchar.h>
#include <algorithm>
#include <string>
#include <array>
#include <vector>
#include <exception>
#include <functional>
#include <memory>

//library includes
// #include <GL/glew.h>
// #include <SDL2/SDL.h>
// #if ASDF_BOX2D == 1
// #include <Box2D/Box2D.h>
// #endif

#ifdef _MSC_VER
#define MSVC
#else
#undef MSVC
#endif

//msvc constexpr is still really lacking
#ifdef MSVC
#define MSVC_CONSTEXPR inline
#else
#define MSVC_CONSTEXPR constexpr
#endif

#ifdef MSVC
#define STRERROR(errno, buffer, buffsize) strerror_s(buffer, buffsize, errno); //MSVC
#define SNPRINTF(str, size, msg, ...) sprintf_s(str, size, msg, __VA_ARGS__);
#else
#define STRERROR(errno, buffer, buffsize) strerror_r(errno, buffer, buffsize); //UNIX
#define SNPRINTF(str, size, msg, ...) snprintf(str, size, msg, __VA_ARGS__);
#endif


#ifdef __clang__
#define CLANG_DIAGNOSTIC_PUSH _Pragma(clang diagnostic push);
#define CLANG_DIAGNOSTIC_POP _Pragma(clang diagnostic pop);
#define CLANG_DIAGNOSTIC_IGNORE(to_ignore) _Pragma(clang diagnostic ignored to_ignore);
#else
#define CLANG_DIAGNOSTIC_PUSH 
#define CLANG_DIAGNOSTIC_POP
#define CLANG_DIAGNOSTIC_IGNORE(to_ignore)
#endif

#ifdef __clang__
#define FALLTHROUGH [[clang::fallthrough]]
#else
#define FALLTHROUGH
#endif

#define GLM_FORCE_CXX11
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/swizzle.hpp> 

//MISC
#define PI 3.14159265359f
#define nullindex 4294967295

//LOGGING MACROS     ## before VA_ARGS required by clang/gcc
#define LOG(msg, ...) do{printf(msg, ##__VA_ARGS__); printf("\n");} while(0);
#define LOG_IF(condition, msg, ...) do{ if(condition){LOG(msg, ##__VA_ARGS__);} }while(0);


//MACRO HELPERS
#ifdef __llvm__
    #define LAX_MACROS 0
#else
    #define LAX_MACROS 1
#endif


//macro argument counter
//http://stackoverflow.com/questions/2308243/macro-returning-the-number-of-arguments-it-is-given-in-c
//
#define PP_NARG(...) \
    PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
    PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
     _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
    _61,_62,_63,  N, ...) N
#define PP_RSEQ_N() \
    63,62,61,60,                   \
    59,58,57,56,55,54,53,52,51,50, \
    49,48,47,46,45,44,43,42,41,40, \
    39,38,37,36,35,34,33,32,31,30, \
    29,28,27,26,25,24,23,22,21,20, \
    19,18,17,16,15,14,13,12,11,10, \
     9, 8, 7, 6, 5, 4, 3, 2, 1, 0


#define MACRO_CONCAT_HELPER(a,b) a##b
#define MACRO_CONCAT(a,b) MACRO_CONCAT_HELPER(a,b)

#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,N,...) N
#define VA_NUM_ARGS_IMPL_(tuple) VA_NUM_ARGS_IMPL tuple
#define VA_NARGS(...)  bool(#__VA_ARGS__) ? (VA_NUM_ARGS_IMPL_((__VA_ARGS__, 24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1))) : 0

#define ENDOF(container) container[container.size() - 1]

#define SELMACRO_IMPL_2(_1,_2, N,...) N
#define SELMACRO_IMPL_3(_1,_2,_3, N,...) N

// Works in MSVC
// http://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros answer by Syphorlate
#define SELMACRO_IMPL_2_(stuff) SELMACRO_IMPL_2 stuff
#define SELMACRO_IMPL_3_(stuff) SELMACRO_IMPL_3 stuff


//ASSET PATH MACROS
// #define ASSETS_PATH "../../assets/"
// #define ASSET_PATH(asset_filename) (MACRO_CONCAT(ASSETS_PATH, asset_filename))

// #define TEXTURES_PATH MACRO_CONCAT(ASSETS_PATH,"textures/")
// #define TEXTURE_PATH(texture_filename) (MACRO_CONCAT(TEXTURES_PATH, texture_filename))

// #define SHADER_DEFAULT_VER 120
// #define SHADERS_PATH "../../shaders/"
// #define SHADERS_PATH_(glsl_ver) MACRO_CONCAT(SHADERS_PATH, MACRO_CONCAT(#glsl_ver,"/"))

// #define SHADER_FILENAME(shader_name, glsl_ver, ext) MACRO_CONCAT(shader_name, MACRO_CONCAT(MACRO_CONCAT("_",#glsl_ver), ext))

// #define SHADER_PATH_3(shader_name, glsl_ver, ext)   (MACRO_CONCAT(SHADERS_PATH_(glsl_ver), SHADER_FILENAME(shader_name, glsl_ver, ext)))
// #define SHADER_PATH_2(shader_filename, glsl_ver)    (MACRO_CONCAT(SHADERS_PATH_(glsl_ver), shader_filename))
// #define SHADER_PATH_1(shader_filename)              (MACRO_CONCAT(SHADERS_PATH_(SHADER_DEFAULT_VER), shader_filename))
// #if(LAX_MACROS)
// #define SHADER_PATH(...) SELMACRO_IMPL_3_((##__VA_ARGS__, SHADER_PATH_3(__VA_ARGS__), SHADER_PATH_2(__VA_ARGS__), SHADER_PATH_1(__VA_ARGS__))) 
// #else
// #define SHADER_PATH(...) SELMACRO_IMPL_3(##__VA_ARGS__, SHADER_PATH_3(__VA_ARGS__), SHADER_PATH_2(__VA_ARGS__), SHADER_PATH_1(__VA_ARGS__))
// #endif

// #define VSHADER_PATH_1(shader_name)               (SHADER_PATH(shader_name, SHADER_DEFAULT_VER, ".vert"))
// #define VSHADER_PATH_2(shader_name, glsl_ver)    (SHADER_PATH(shader_name, glsl_ver, ".vert"))
// #if(LAX_MACROS)
// #define VSHADER_PATH(...) SELMACRO_IMPL_2_((__VA_ARGS__, VSHADER_PATH_2(__VA_ARGS__), VSHADER_PATH_1(__VA_ARGS__))) 
// #else
// #define VSHADER_PATH(...) SELMACRO_IMPL_2(__VA_ARGS__, VSHADER_PATH_2(__VA_ARGS__), VSHADER_PATH_1(__VA_ARGS__))
// #endif

// #define FSHADER_PATH_1(shader_name)               (SHADER_PATH(shader_name, SHADER_DEFAULT_VER, ".frag"))
// #define FSHADER_PATH_2(shader_name, glsl_ver)    (SHADER_PATH(shader_name, glsl_ver, ".frag"))
// #if(LAX_MACRO)
// #define FSHADER_PATH(...) SELMACRO_IMPL_2_((__VA_ARGS__, FSHADER_PATH_2(__VA_ARGS__), FSHADER_PATH_1(__VA_ARGS__)))
// #else
// #define FSHADER_PATH(...) SELMACRO_IMPL_2(__VA_ARGS__, FSHADER_PATH_2(__VA_ARGS__), FSHADER_PATH_1(__VA_ARGS__))
// #endif

// #define FONTS_PATH MACRO_CONCAT(ASSETS_PATH,"fonts/")
// #define FONT_PATH(font_filename) (MACRO_CONCAT(FONTS_PATH, font_filename))


//useful includes
// #include "asdf_multiplat.h"
// #include "texture.h"
// #include "spritebatch.h"
// #include "content_manager.h"
// #include "settings.h"
// #include "utilities.h"

//input includes
//#include "mouse_state.h"
//#include "keyboardinfo.h"


//Assertion code shamelessly copymodified from: 
//http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/

#define ASSERTS_ENABLED

//void asdf_fail(char const* condition, char const* file, int line, char const* msg, ...);
void asdf_fail(char const* condition, char const* file, int line, ...);

#ifdef ASSERTS_ENABLED  
    #define ASSERT(cond, ...) \
    do { \
        if (!(cond)) { \
            LOG("ASSERT FAILED: %s, %s, %i", #cond, __FILE__, __LINE__) \
            LOG(__VA_ARGS__); \
            /*__debugbreak();*/ \
        } \
    } while(0)  
#else  
    #define ASSERT(condition) \  
do { POW2_UNUSED(condition); } while (0)
#endif 




// #define COLOR_CLEAR     color_t(0.0f, 0.0f, 0.0f, 1.0f)
// #define COLOR_WHITE		color_t(1.0f, 1.0f, 1.0f, 1.0f)
// #define COLOR_LIGHTGREY color_t(0.8f, 0.8f, 0.8f, 1.0f)
// #define COLOR_LIGHTGRAY COLOR_LIGHTGREY
// #define COLOR_GREY	    color_t(0.5f, 0.5f, 0.5f, 1.0f)
// #define COLOR_GRAY      COLOR_GREY
// #define COLOR_DARKGREY  color_t(0.2f, 0.2f, 0.2f, 1.0f)
// #define COLOR_DARKGRAY  COLOR_DARKGREY
// #define COLOR_BLACK		color_t(0.0f, 0.0f, 0.0f, 1.0f)

// #define COLOR_RED		color_t(1.0f, 0.0f, 0.0f, 1.0f)
// #define COLOR_GREEN		color_t(0.0f, 1.0f, 0.0f, 1.0f)
// #define COLOR_BLUE		color_t(0.0f, 0.0f, 1.0f, 1.0f)
// #define COLOR_CYAN		color_t(0.0f, 1.0f, 1.0f, 1.0f)
// #define COLOR_YELLOW	color_t(1.0f, 1.0f, 0.0f, 1.0f)
// #define COLOR_MAGENTA	color_t(1.0f, 0.0f, 1.0f, 1.0f)

// #define COLOR_TEAL            color_t(0.0f, 1.0f, 0.5f, 1.0f)
// #define COLOR_ORANGE          color_t(1.0f, 0.5f, 0.0f, 1.0f)
// #define COLOR_CORNFLOWER_BLUE color_t(0.5f, 0.75f, 0.9f, 1.0f)
