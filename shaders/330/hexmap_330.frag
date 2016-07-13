#version 330 core
// Hexmap Shader

smooth in vec2 FragTexCoord;
flat in vec4 ColorOut;

layout(location = 0) out vec4 FragColor;

uniform sampler2D TextureMap;
 
void main() {
	//vec4 texcol = texture2D(TextureMap, FragTexCoord);
	//FragColor = texcol * ColorOut;
    FragColor = ColorOut;
	//FragColor.a = 1.0f;
}