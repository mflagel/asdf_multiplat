#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 VertColor;

out vec4 ColorOut;

uniform mat4 WVP;

void main(void)
{
	gl_Position = WVP * vec4(Position, 1.0);
    ColorOut = VertColor;
}
