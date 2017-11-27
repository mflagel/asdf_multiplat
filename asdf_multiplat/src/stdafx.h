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


#define GLM_FORCE_CXX11
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/glm.hpp>



#include "main/asdf_defs.h"
#include "utilities/macro_overloading.h"



//ASSET PATH MACROS
// #define ASSETS_PATH "../../assets/"
// #define ASSET_PATH(asset_filename) (MACRO_CONCAT(ASSETS_PATH, asset_filename))

// #define TEXTURES_PATH MACRO_CONCAT(ASSETS_PATH,"textures/")
// #define TEXTURE_PATH(texture_filename) (MACRO_CONCAT(TEXTURES_PATH, texture_filename))

// #define SHADER_DEFAULT_VER 120
// #define SHADERS_PATH "../../shaders/"
// #define SHADERS_PATH_(glsl_ver) MACRO_CONCAT(SHADERS_PATH, MACRO_CONCAT(#glsl_ver,"/"))

// #define FONTS_PATH MACRO_CONCAT(ASSETS_PATH,"fonts/")
// #define FONT_PATH(font_filename) (MACRO_CONCAT(FONTS_PATH, font_filename))
