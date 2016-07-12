#version 330 core
// Hexmap Shader

layout(location = 0) in vec4 VertexPosition;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec4 VertexColor;


smooth out vec2 FragTexCoord;
flat out vec4 ColorOut;

uniform mat4 WVP;

void main(void)
{
    vec4 pos = VertexPosition;
    pos.x += 1.5 * gl_InstanceID;

	gl_Position = WVP * pos;
	FragTexCoord = TexCoord;
	ColorOut = VertexColor;
}
