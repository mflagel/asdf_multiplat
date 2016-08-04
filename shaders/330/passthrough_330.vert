#version 330 core

layout(location = 0) in vec3 VertexPosition;

out vec4 ColorOut;
out vec2 FragTexCoord;

uniform mat4 WVP;

void main(void)
{
    FragTexCoord = VertexPosition.xy;

	vec4 pos = vec4(1);
	pos.xyz = VertexPosition;
	gl_Position = WVP * pos;
}
