#include "stdafx.h"
#include <stdio.h>
#include "utilities_openGL.h"
//#include "shader.h"

namespace asdf {
    namespace util {

        bool CheckGLError(GLuint shader) {
        GLint glStatus;
        GLchar *glErrorMsg = 0;
        GLsizei glErrLen = 256;

        if (shader != nullindex) {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &glStatus);
            if (glStatus == GL_FALSE) {
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &glErrLen);
                glErrorMsg = (GLchar *)malloc((glErrLen + 50)*sizeof(char));
                //SNPRINTF(glErrorMsg, glErrLen + 50, "%s\n", "Failed to compile the shader!");
                glGetShaderInfoLog(shader, glErrLen, nullptr, &glErrorMsg[strlen(glErrorMsg)]);
                fprintf(stderr, "%s\n", glErrorMsg);
                free(glErrorMsg);
                return true;
            }
        }

        GLenum errCode;
        const GLubyte *errString;

        if ((errCode = glGetError()) != GL_NO_ERROR) {
            errString = gluErrorString(errCode);
            fprintf(stderr, "OpenGL Error: %s\n", errString);
            return true;
        }

        return false;
    }

    }
}