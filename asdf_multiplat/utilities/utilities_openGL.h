#pragma once

#include <GL/glew.h>

namespace asdf {
    namespace util {

    bool CheckShader(GLuint shader);
    bool CheckGLError(GLuint shader/* = 0xFFFFFFFF*/);

    // for some absurd reason, msvc is saying the default arg is redefined so I have to overload
    bool CheckGLError();

    }
}

//cube data from the XCode OpenGL ES2 base project
//GLfloat gCubeVertexData[108] = 
//{
//	// Data layout for each line below is:
//	// positionX, positionY, positionZ
//	0.5f, -0.5f, -0.5f,
//	0.5f, 0.5f, -0.5f,
//	0.5f, -0.5f, 0.5f,
//	0.5f, -0.5f, 0.5f,
//	0.5f, 0.5f, -0.5f,
//	0.5f, 0.5f, 0.5f,
//
//	0.5f, 0.5f, -0.5f,
//	-0.5f, 0.5f, -0.5f,
//	0.5f, 0.5f, 0.5f,
//	0.5f, 0.5f, 0.5f,
//	-0.5f, 0.5f, -0.5f,
//	-0.5f, 0.5f, 0.5f,
//
//	-0.5f, 0.5f, -0.5f,
//	-0.5f, -0.5f, -0.5f,
//	-0.5f, 0.5f, 0.5f,
//	-0.5f, 0.5f, 0.5f,
//	-0.5f, -0.5f, -0.5f,
//	-0.5f, -0.5f, 0.5f,
//
//	-0.5f, -0.5f, -0.5f,
//	0.5f, -0.5f, -0.5f,
//	-0.5f, -0.5f, 0.5f,
//	-0.5f, -0.5f, 0.5f,
//	0.5f, -0.5f, -0.5f,
//	0.5f, -0.5f, 0.5f,
//
//	0.5f, 0.5f, 0.5f,
//	-0.5f, 0.5f, 0.5f,
//	0.5f, -0.5f, 0.5f,
//	0.5f, -0.5f, 0.5f,
//	-0.5f, 0.5f, 0.5f,
//	-0.5f, -0.5f, 0.5f,
//
//	0.5f, -0.5f, -0.5f,
//	-0.5f, -0.5f, -0.5f,
//	0.5f, 0.5f, -0.5f,
//	0.5f, 0.5f, -0.5f,
//	-0.5f, -0.5f, -0.5f,
//	-0.5f, 0.5f, -0.5f,
//};
//
//
//GLfloat gCubeVertexNormalData[216] = 
//{
//	// Data layout for each line below is:
//	// positionX, positionY, positionZ,     normalX, normalY, normalZ,
//	0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,
//	0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,
//	0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,
//	0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,
//	0.5f, 0.5f, -0.5f,          1.0f, 0.0f, 0.0f,
//	0.5f, 0.5f, 0.5f,         1.0f, 0.0f, 0.0f,
//
//	0.5f, 0.5f, -0.5f,         0.0f, 1.0f, 0.0f,
//	-0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
//	0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,
//	0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,
//	-0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
//	-0.5f, 0.5f, 0.5f,         0.0f, 1.0f, 0.0f,
//
//	-0.5f, 0.5f, -0.5f,        -1.0f, 0.0f, 0.0f,
//	-0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
//	-0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,
//	-0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,
//	-0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
//	-0.5f, -0.5f, 0.5f,        -1.0f, 0.0f, 0.0f,
//
//	-0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,
//	0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
//	-0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,
//	-0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,
//	0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
//	0.5f, -0.5f, 0.5f,         0.0f, -1.0f, 0.0f,
//
//	0.5f, 0.5f, 0.5f,          0.0f, 0.0f, 1.0f,
//	-0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
//	0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
//	0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
//	-0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
//	-0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
//
//	0.5f, -0.5f, -0.5f,        0.0f, 0.0f, -1.0f,
//	-0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
//	0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,
//	0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,
//	-0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
//	-0.5f, 0.5f, -0.5f,        0.0f, 0.0f, -1.0f
//};