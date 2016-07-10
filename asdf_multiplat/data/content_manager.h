#pragma once

#include <memory>
#include <FTGL/ftgl.h>
#include "resource_depot.h"
#include "utilities/utilities.h"
#include "utilities/utilities_openGL.h"
#include "shader.h"
#include "texture.h"

 
#define PIXEL_TEX ContentManager::GetTexture("pixel")


namespace asdf {
    class content_manager_t {
    public:
        std::string shader_path;

        content_manager_t();
        ~content_manager_t();

        resource_depot_t<std::shared_ptr<texture_t>> textures;
        resource_depot_t<std::shared_ptr<shader_t>> shaders;
        resource_depot_t<GLuint> samplers;
        resource_depot_t<FTFont*> fonts; //use raw ptr, as FTGL explodes if I use shared ptr for some reason. might try unique_ptr

        void init();

        bool load_shader(std::string const& name, size_t glsl_ver);

        int AddTexturesFromFolder(const char* assetsFolder); //adds jpg, png		
        int AddShadersFromFolder(const char* assetsFolder); //adds .vert and .frag and compiles them into shader programs
        //int AddAudioFromFolder(std::string folderPath); //adds audio files
        void AddSamplers();
    };

    template <class FNT = FTPixmapFont>
    FTFont* load_font(const char* font_file_path)
    {
        FTFont* loaded_font = new FNT(font_file_path);

        if(auto error = loaded_font->Error())
        {
            throw content_load_exception(font_file_path, "font load error: " + std::to_string(error)); //todo: spit out error message from fterrdefs.h
        }

        return loaded_font;
    }

    extern content_manager_t Content;
}