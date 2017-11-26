#include "stdafx.h"
#include "content_manager.h"
#include <sstream>
#include <unordered_map>
#include <SOIL/SOIL.h>

#include "utilities/utilities.h"
#include "data/gl_state.h"

using namespace std;
//using namespace boost::filesystem;

namespace asdf
{
    using namespace util;

    ///
    ASDFM_API content_manager_t Content;
    ///

    content_load_exception::content_load_exception(std::string const& _filepath, std::string const& _loadError) 
        : filepath(_filepath)
        , loadError(_loadError) 
    {}

    const char* content_load_exception::what() const noexcept
    { 
        return ("Unable to load content file: " + filepath + "\nLoad Error:" + loadError + "\n").c_str();
    }

    resource_not_found_exception::resource_not_found_exception(std::string _resourceName)
        : std::runtime_error(std::string("Could not find resource: ") + _resourceName)
        , resourceName(_resourceName)
    {}

    // const char* resource_not_found_exception::what() const 
    // {
    //     return ("Could not find resource: " + resourceName).c_str();
    // }


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

        /// Textures
        load_texture("pixel", "pixel.bmp");
        //ADD_TEXTURE("debug", "debug_purple.png");
        //ADD_TEXTURE("pixel", "pixel.bmp");
        //ADD_TEXTURE("particle_test", "particle_test_texture.png");

        /// Shaders
        shaders.add_resource(create_shader_highest_supported("passthrough"));
        // shaders.add_resource(create_shader_highest_supported("colored", 330));
        shaders.add_resource(create_shader_highest_supported("spritebatch"));

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

    void content_manager_t::load_texture(std::string const& name, std::string const& relative_path)
    {
        auto tex_path = string(asset_path + "/" + relative_path);

        auto tex = make_shared<texture_t>(name, tex_path);
        textures.add_resource(tex);
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

    std::shared_ptr<shader_t> content_manager_t::create_shader(std::string const& _shader_path, std::string const& vs_name, std::string const& fs_name, size_t glsl_ver)
    {
        ASSERT(_shader_path.length() > 0, "Loading shader before shader path is set");

        string ver_str = to_string(glsl_ver);
        auto ver_path = string(_shader_path + "/" + ver_str);                    // ex: ".../shaders/330"

        auto vshd_path = string(ver_path + "/" + vs_name + "_" + ver_str + ".vert");   // .../shaders/330/shadername_330
        auto fshd_path = string(ver_path + "/" + fs_name + "_" + ver_str + ".frag");

        string name(vs_name);

        if(vs_name != fs_name)
            name += string("+" + fs_name);

        auto shader = make_shared<shader_t>(std::move(name), vshd_path, fshd_path);

        return shader;
    }

    std::shared_ptr<shader_t> content_manager_t::create_shader_highest_supported(std::string const& name)
    {
        return create_shader_highest_supported(name, name);
    }

    std::shared_ptr<shader_t> content_manager_t::create_shader_highest_supported(std::string const& vs_name, std::string const& fs_name)
    {
        return create_shader(vs_name, fs_name, GL_State->highest_glsl_version);
    }

    std::shared_ptr<shader_t> content_manager_t::create_shader_highest_supported(std::string const& _shader_path, std::string const& vs_name, std::string const& fs_name)
    {
        return create_shader(_shader_path, vs_name, fs_name, GL_State->highest_glsl_version);
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