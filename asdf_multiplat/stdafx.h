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



#include "main/asdf_defs.h"
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


#ifdef __clang__
#define PRAGMA(x) _Pragma(#x)
#define CLANG_DIAGNOSTIC_PUSH _Pragma("clang diagnostic push");
#define CLANG_DIAGNOSTIC_POP _Pragma("clang diagnostic pop");
#define CLANG_DIAGNOSTIC_IGNORE(to_ignore) PRAGMA(clang diagnostic ignored to_ignore);
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
