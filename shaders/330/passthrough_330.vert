#version 330 core

layout(location = 0) in vec3 VertexPosition;

out vec4 ColorOut;
out vec2 FragTexCoord;

uniform mat4 WVP;

void main(void)
{
    FragTexCoord = VertexPosition.xy + vec2(0.5, 0.5);

	vec4 pos = vec4(1);
	pos.xyz = VertexPosition;
    vec4 pos_test = WVP * pos;
	gl_Position = WVP * pos;
}
