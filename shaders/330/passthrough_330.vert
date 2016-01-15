#version 330 core

//layout(location = 0) in vec4 Position;
layout(location = 0) in vec2 Position;

out vec4 ColorOut;

uniform mat4 WVP;

void main(void)
{
	vec4 pos = vec4(1);
	pos.xy = Position;
	gl_Position = WVP * pos;
	//gl_Position = WVP * Position;
}
