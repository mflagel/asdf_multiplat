#version 330 core

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

smooth out vec2 TexCoordOut;

uniform mat4 WVP;

void main(void)
{
	gl_Position = WVP * Position;
	TexCoordOut = TexCoord;
}
