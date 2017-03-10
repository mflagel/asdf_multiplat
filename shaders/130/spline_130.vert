#version 130
// Spline Vertex Shader

in vec2 VertexPosition;
in vec2 VertexNormal;
in vec4 VertexColor;


//smooth out vec2 FragTexCoord;
smooth out vec4 ColorOut;
smooth out vec2 NormalOut;

uniform mat4 WVP;

const float LineThickness = 0.1;  //todo: replace constant with thickness

void main(void)
{
    /// Position
    vec4 pos = vec4(VertexPosition + VertexNormal * LineThickness, 0.0, 1.0);
    gl_Position = WVP * pos;

    ColorOut = VertexColor;
    NormalOut = VertexNormal;
}
