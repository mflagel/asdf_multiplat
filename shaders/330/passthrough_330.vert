#version 330 core

//layout(location = 0) in vec4 Position;
layout(location = 0) in vec2 VertexPosition;

out vec4 ColorOut;
out vec2 FragTexCoord;

uniform mat4 WVP;

void main(void)
{
    FragTexCoord = VertexPosition.xy;

	vec4 pos = vec4(1);
	pos.xy = VertexPosition;
	gl_Position = WVP * pos;
}
