#version 330 core
// Spline Vertex Shader

layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec2 VertexNormal;
layout(location = 2) in vec4 VertexColor;


//smooth out vec2 FragTexCoord;
smooth out vec4 ColorOut;
smooth out vec2 NormalOut;

uniform mat4 WVP;

const float LineThickness = 0.1;  //todo: replace constant with thickness

void main(void)
{
    /// Position
    vec4 pos = vec4(VertexPosition, 0.0, 1.0);
    gl_Position = WVP * pos;
    gl_Position += vec4(VertexNormal, 0.0, 1.0) * LineThickness;

    ColorOut = VertexColor;
    NormalOut = VertexNormal;
}
