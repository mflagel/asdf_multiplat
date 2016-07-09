#include "stdafx.h"

#include <stdio.h>
#include <string>

#include "utilities_openGL.h"
//#include "shader.h"

namespace asdf {
    namespace util {

        /// I'm not entirely sure why these functions return true if there's an error, but whatever

        bool CheckShader(GLuint shader)
        {
            GLint glStatus;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &glStatus);

            if(glStatus == GL_FALSE)
            {
                GLint error_length = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_length);

                std::string gl_error_str(error_length, '\n');
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

        bool CheckGLError() { return CheckGLError(0xFFFFFFFF); }


    }
}