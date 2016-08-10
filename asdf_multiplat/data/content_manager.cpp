#include "stdafx.h"
#include "content_manager.h"
#include <sstream>
#include <unordered_map>
#include <SOIL/SOIL.h>

#include "utilities/utilities.h"

using namespace std;
//using namespace boost::filesystem;

namespace asdf
{
    using namespace util;

    ///
    content_manager_t Content;
    ///

    content_load_exception::content_load_exception(std::string const& filepath, std::string const& loadError) 
        : filepath(filepath), loadError(loadError) 
    {}

    const char* content_load_exception::what() const noexcept
    { 
        return ("Unable to load content file: " + filepath + "\nLoad Error:" + loadError + "\n").c_str();
    }

    resource_not_found_exception::resource_not_found_exception(std::string resourceName)
        : resourceName(resourceName)
    {}

    const char* resource_not_found_exception::what() const 
    {
        return ("Could not find resource: " + resourceName).c_str();
    }


    content_manager_t::content_manager_t()
    {
    }
    
    content_manager_t::~content_manager_t()
    {
    }

    void content_manager_t::init()
    {
        LOG("Initializing Content");

        asset_path = find_folder("assets");
        shader_path = find_folder("shaders");

        LOG_IF(asset_path.length() > 0, "Could not find asset folder");
        LOG_IF(shader_path.length() > 0, "Could not find shader folder");

        //textures
        //ADD_TEXTURE("debug", "debug_purple.png");
        //ADD_TEXTURE("pixel", "pixel.bmp");
        //ADD_TEXTURE("particle_test", "particle_test_texture.png");

        // //shaders
        shaders.add_resource(create_shader("passthrough", 330));
        shaders.add_resource(create_shader("colored", 330));
        shaders.add_resource(create_shader("spritebatch", 330));

        shaders.default_resource = shaders["passthrough"];


        //fonts
        //fonts.add_resource( "arial",   new FTPixmapFont(FONT_PATH("arial.ttf")) );
        //fonts.add_resource( "consolas", new FTPixmapFont(FONT_PATH("Consolas.ttf")) );

        //fonts["arial"]->FaceSize(72);
        //fonts["consolas"]->FaceSize(72);
        //fonts.default_resource = fonts["consolas"];

        AddSamplers();

        LOG("Content Initialized");
    }



    shared_ptr<shader_t> content_manager_t::create_shader(string const& name, size_t glsl_ver)
    {
        auto shader = create_shader(name, name, glsl_ver);
        shader->name = name;
        return shader;
    }

    shared_ptr<shader_t> content_manager_t::content_manager_t::create_shader(std::string const& vs_name, std::string const& fs_name, size_t glsl_ver)
    {
        return create_shader(shader_path, vs_name, fs_name, glsl_ver);
    }

    std::shared_ptr<shader_t> content_manager_t::create_shader(std::string const& shader_path, std::string const& vs_name, std::string const& fs_name, size_t glsl_ver)
    {
        ASSERT(shader_path.length() > 0, "Loading shader before shader path is set");

        string ver_str = to_string(glsl_ver);
        auto ver_path = string(shader_path + "/" + ver_str);                    // ex: ".../shaders/330"

        auto vshd_path = string(ver_path + "/" + vs_name + "_" + ver_str + ".vert");   // .../shaders/330/shadername_330
        auto fshd_path = string(ver_path + "/" + fs_name + "_" + ver_str + ".frag");

        auto shader = make_shared<shader_t>("", vshd_path, fshd_path);

        return shader;
    }

    //int content_manager_t::AddTexturesFromFolder(string folderPath) {
    //    if (!is_directory(folderPath)) {
    //        std::cout << "Cannot add textures from non-existant folder: " << folderPath;
    //        return 0;
    //    }


    //    size_t numLoadedTextures = 0;

    //    std::for_each(recursive_directory_iterator(folderPath), recursive_directory_iterator(), [&](directory_entry dirEnt) {
    //        path filepath = dirEnt.path();
    //        path ext = filepath.extension();
    //        if (ext == ".png" || ext == ".jpg" || ext == ".bmp") {
    //            string textureFilename = filepath.filename().generic_string(); //get filename with ext
    //            string textureName = textureFilename.substr(0, textureFilename.length() - 4); //chop of ext

    //            Texture* loadedTexture = new Texture(textureName, filepath.generic_string());
    //            textures.AddResource(textureName, loadedTexture);
    //            numLoadedTextures++;
    //        }
    //    });

    //    return numLoadedTextures;
    //}

    void content_manager_t::AddSamplers(){
    	GLuint tempSamplers[2];
    	glGenSamplers(2, tempSamplers);
    
    	//linear clamp
    	glSamplerParameteri(tempSamplers[0], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    	glSamplerParameteri(tempSamplers[0], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	glSamplerParameteri(tempSamplers[0], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    	glSamplerParameteri(tempSamplers[0], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    	samplers.add_resource("LinearClamp", tempSamplers[0]);
    
    	//linear wrap
    	glSamplerParameteri(tempSamplers[1], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    	glSamplerParameteri(tempSamplers[1], GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
    	glSamplerParameteri(tempSamplers[1], GL_TEXTURE_WRAP_S, GL_REPEAT);
    	glSamplerParameteri(tempSamplers[1], GL_TEXTURE_WRAP_T, GL_REPEAT);
    	samplers.add_resource("LinearRepeat", tempSamplers[1]);

        samplers.default_resource = tempSamplers[0];
    
    	ASSERT(!CheckGLError(), "Error loading samplers");
    }
    //
    ////void ContentManager::AddPixelTexture(){
    ////	char pixel[] = {127,127,127,255};
    ////	GLuint pixelIndex;
    ////	glGenTextures(1, &pixelIndex);
    ////	glBindTexture(GL_TEXTURE_2D, pixelIndex);
    ////	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixel);
    ////	glBindTexture(GL_TEXTURE_2D, 0);
    ////	CheckGLError();
    ////
    ////	textures.AddResource( "pixel", new Texture("pixel", pixelIndex) );
    ////}
}