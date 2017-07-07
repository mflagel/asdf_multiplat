#include "stdafx.h"
#include "gl_resources.h"
#include "gl_state.h"

namespace asdf
{
    using namespace util;


    render_target_t::render_target_t(uint32_t width, uint32_t height)
    : fbo{}
    , texture{"render_target_t texture", nullptr, width, height}
    {
        ASSERT(!CheckGLError(), "GL Error Creating Render Target");
    }

    void render_target_t::init()
    {
        ASSERT(!CheckGLError(), "GL Error **Before** Initializing Render Target");

        GL_State->init_render_target(fbo, texture);

        ASSERT(!CheckGLError(), "GL Error Initializing Render Target");
    }



    /// I'm not entirely sure why these functions return true if there's an error, but whatever

    bool CheckShader(GLuint shader)
    {
        GLint glStatus;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &glStatus);

        if(glStatus == GL_FALSE)
        {
            GLint error_length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_length);

            ASSERT(error_length >= 0, "Invalid GL Error Length");

            std::string gl_error_str(size_t(error_length), '\n');
            glGetShaderInfoLog(shader, error_length, nullptr, &(gl_error_str[0]));

            fprintf(stderr, "Failed to compile shader!\n%s\n", gl_error_str.data());
            return true;
        }

        return false;
    }

    bool CheckGLError(GLuint shader)
    {
        bool had_error = false;

        if(shader != nullindex)
        {
            had_error = CheckShader(shader);
        }

        GLenum errCode = GL_NO_ERROR;
        
        while((errCode = glGetError()) != GL_NO_ERROR)
        {
            const GLubyte *errString;
            errString = gluErrorString(errCode);
            fprintf(stderr, "OpenGL Error: %s\n", errString);
            had_error = true;
        }

        return had_error;
    }

    bool CheckGLError(){ return CheckGLError(0xFFFFFFFF); }

    bool check_attrib_error()
    {
        return CheckGLError();
    }
}