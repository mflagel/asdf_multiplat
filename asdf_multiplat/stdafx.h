#pragma once

//#include <algorithm>
//#include <string>
//#include <array>
//#include <vector>
//#include <exception>
//#include <functional>
//#include <memory>


#include <GL/glew.h>
#include <SDL2/SDL.h>

#define ASDF_SDL2 1


#define GLM_FORCE_CXX11
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/glm.hpp>



//MISC
#define PI 3.14159265359f
#define nullindex 4294967295

#define ENDOF(container) container[container.size() - 1]


//LOGGING MACROS     ## before VA_ARGS required by clang/gcc
#define LOG(msg, ...) do{printf(msg, ##__VA_ARGS__); printf("\n");} while(0);
#define LOG_IF(condition, msg, ...) do{ if(condition){LOG(msg, ##__VA_ARGS__);} }while(0);



// Color Stuff
using color_t = glm::vec4;

#define COLOR_CLEAR     color_t(0.0f, 0.0f, 0.0f, 1.0f)
#define COLOR_WHITE		color_t(1.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_LIGHTGREY color_t(0.8f, 0.8f, 0.8f, 1.0f)
#define COLOR_LIGHTGRAY COLOR_LIGHTGREY
#define COLOR_GREY	    color_t(0.5f, 0.5f, 0.5f, 1.0f)
#define COLOR_GRAY      COLOR_GREY
#define COLOR_DARKGREY  color_t(0.2f, 0.2f, 0.2f, 1.0f)
#define COLOR_DARKGRAY  COLOR_DARKGREY
#define COLOR_BLACK		color_t(0.0f, 0.0f, 0.0f, 1.0f)

#define COLOR_RED		color_t(1.0f, 0.0f, 0.0f, 1.0f)
#define COLOR_GREEN		color_t(0.0f, 1.0f, 0.0f, 1.0f)
#define COLOR_BLUE		color_t(0.0f, 0.0f, 1.0f, 1.0f)
#define COLOR_CYAN		color_t(0.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_YELLOW	color_t(1.0f, 1.0f, 0.0f, 1.0f)
#define COLOR_MAGENTA	color_t(1.0f, 0.0f, 1.0f, 1.0f)

#define COLOR_TEAL            color_t(0.0f, 1.0f, 0.5f, 1.0f)
#define COLOR_ORANGE          color_t(1.0f, 0.5f, 0.0f, 1.0f)
#define COLOR_CORNFLOWER_BLUE color_t(0.5f, 0.75f, 0.9f, 1.0f)


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
            __debugbreak(); \
        } \
    } while(0)

    //like assert(false, ...)
    #define EXPLODE(message, ...) \
        do \
        { \
            LOG("EXPLOSION: %s, %i", __FILE__, __LINE__); \
            LOG(message, __VA_ARGS__);\
            __debugbreak(); \
        } while(0)
#else  
    #define ASSERT(condition) do { POW2_UNUSED(condition); } while (0)
    #define EXPLODE(message, ...) do { POW2_UNUSED(condition); } while (0)
#endif 




#include "utilities/macro_overloading.h"


#ifdef __llvm__
    #define LAX_MACROS 0
#else
    #define LAX_MACROS 1
#endif



//TODO: just go and replace usages of macro_concat with macro pasting. I forget why I have this. for outdated compilers maybe?
#define MACRO_CONCAT_HELPER(a,b) a##b
#define MACRO_CONCAT(a,b) MACRO_CONCAT_HELPER(a,b)


//ASSET PATH MACROS
#define ASSETS_PATH "../../assets/"
#define ASSET_PATH(asset_filename) (MACRO_CONCAT(ASSETS_PATH, asset_filename))

#define TEXTURES_PATH MACRO_CONCAT(ASSETS_PATH,"textures/")
#define TEXTURE_PATH(texture_filename) (MACRO_CONCAT(TEXTURES_PATH, texture_filename))

#define SHADER_DEFAULT_VER 120
#define SHADERS_PATH "../../shaders/"
#define SHADERS_PATH_(glsl_ver) MACRO_CONCAT(SHADERS_PATH, MACRO_CONCAT(#glsl_ver,"/"))

#define SHADER_FILENAME(shader_name, glsl_ver, ext) MACRO_CONCAT(shader_name, MACRO_CONCAT(MACRO_CONCAT("_",#glsl_ver), ext))

#define SHADER_PATH_3(shader_name, glsl_ver, ext)   (MACRO_CONCAT(SHADERS_PATH_(glsl_ver), SHADER_FILENAME(shader_name, glsl_ver, ext)))
#define SHADER_PATH_2(shader_filename, glsl_ver)    (MACRO_CONCAT(SHADERS_PATH_(glsl_ver), shader_filename))
#define SHADER_PATH_1(shader_filename)              (MACRO_CONCAT(SHADERS_PATH_(SHADER_DEFAULT_VER), shader_filename))
#if(LAX_MACROS)
#define SHADER_PATH(...) SELMACRO_IMPL_3_((##__VA_ARGS__, SHADER_PATH_3(__VA_ARGS__), SHADER_PATH_2(__VA_ARGS__), SHADER_PATH_1(__VA_ARGS__))) 
#else
#define SHADER_PATH(...) SELMACRO_IMPL_3(##__VA_ARGS__, SHADER_PATH_3(__VA_ARGS__), SHADER_PATH_2(__VA_ARGS__), SHADER_PATH_1(__VA_ARGS__))
#endif

#define VSHADER_PATH_1(shader_name)               (SHADER_PATH(shader_name, SHADER_DEFAULT_VER, ".vert"))
#define VSHADER_PATH_2(shader_name, glsl_ver)    (SHADER_PATH(shader_name, glsl_ver, ".vert"))
#if(LAX_MACROS)
#define VSHADER_PATH(...) SELMACRO_IMPL_2_((__VA_ARGS__, VSHADER_PATH_2(__VA_ARGS__), VSHADER_PATH_1(__VA_ARGS__))) 
#else
#define VSHADER_PATH(...) SELMACRO_IMPL_2(__VA_ARGS__, VSHADER_PATH_2(__VA_ARGS__), VSHADER_PATH_1(__VA_ARGS__))
#endif

#define FSHADER_PATH_1(shader_name)               (SHADER_PATH(shader_name, SHADER_DEFAULT_VER, ".frag"))
#define FSHADER_PATH_2(shader_name, glsl_ver)    (SHADER_PATH(shader_name, glsl_ver, ".frag"))
#if(LAX_MACRO)
#define FSHADER_PATH(...) SELMACRO_IMPL_2_((__VA_ARGS__, FSHADER_PATH_2(__VA_ARGS__), FSHADER_PATH_1(__VA_ARGS__)))
#else
#define FSHADER_PATH(...) SELMACRO_IMPL_2(__VA_ARGS__, FSHADER_PATH_2(__VA_ARGS__), FSHADER_PATH_1(__VA_ARGS__))
#endif

#define FONTS_PATH MACRO_CONCAT(ASSETS_PATH,"fonts/")
#define FONT_PATH(font_filename) (MACRO_CONCAT(FONTS_PATH, font_filename))





/***  Annoying Cross-Compiler Stuff  ***/
#ifndef _MSC_VER
#define noexcept noexcept
#else
//MSVC is STILL a terrible compiler but apparently I no longer need this? A bunch of stdlib stuff fails to compile if the next line is uncommented
//#define noexcept 
#endif

#ifndef _MSC_VER
#define STRERROR(errno, buffer, buffsize) strerror_r(errno, buffer, buffsize); //UNIX
#define SNPRINTF(str, size, msg, ...) snprintf(str, size, msg, __VA_ARGS__);
#else
#define STRERROR(errno, buffer, buffsize) strerror_s(buffer, buffsize, errno); //MSVC
#define SNPRINTF(str, size, msg, ...) sprintf_s(str, size, msg, __VA_ARGS__);
#endif

