#version 330 core
// Spline Vertex Shader

layout(location = 0) in vec4 VertexPosition;
layout(location = 1) in vec4 VertexColor;


//smooth out vec2 FragTexCoord;
smooth out vec4 ColorOut;

uniform mat4 WVP;

void main(void)
{
    /// Position
    vec4 pos = VertexPosition;
    gl_Position = WVP * pos;

    //todo: move vert pos based on line thickness and direction

    /// Color
    ColorOut = VertexColor;
    //ColorOut.a = 1.0;
}
