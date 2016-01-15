#version 330 core

smooth in vec2 TexCoordOut;
flat in vec4 ColorOut;

layout(location = 0) out vec4 FragColor;

uniform sampler2D TextureMap;
 
void main() {
	vec4 texcol = texture2D(TextureMap, TexCoordOut);
	FragColor = texcol * ColorOut;
	//FragColor.a = 1.0f;
}