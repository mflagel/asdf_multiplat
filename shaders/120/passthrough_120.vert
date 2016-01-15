#version 120

attribute vec4 Position;

uniform mat4 WVP;

void main(void)
{
	gl_Position = WVP * Position;
}
