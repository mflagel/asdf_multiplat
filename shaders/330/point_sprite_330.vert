#version 330

layout(location = 0) in vec4 Position;

uniform mat4 WVP;
uniform mat4 WV;

void main() {	
	gl_Position = WVP * Position;

	float size = 100;
	vec3 attenuation = vec3(0, 1, 0.5f);

	vec4 vertexViewspace = WV * Position;
	float distance = -vertexViewspace.z;
	gl_PointSize = size/(attenuation.x + attenuation.y*distance + attenuation.z*distance*distance);
}
