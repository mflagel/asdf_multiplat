#version 330 core
// Hexmap Shader

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec4 VertColor;

smooth out vec2 TexCoordOut;
flat out vec4 ColorOut;

uniform mat4 WVP;

void main(void)
{
    // gl_InstanceID

	gl_Position = WVP * Position;
	TexCoordOut = TexCoord;
	ColorOut = VertColor;
}
