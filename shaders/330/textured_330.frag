#version 330 core

smooth in vec2 FragTexCoord;

layout(location = 0) out vec4 FragColor;

uniform sampler2D TextureMap;
uniform vec4 Color;
 
void main() {
	vec4 texcol = texture2D(TextureMap, FragTexCoord);
	FragColor = texcol;
	//FragColor = texcol * Color;

	//FragColor = vec4(0,0,0,1);
	//FragColor.r = TexCoordOut.x;
	//FragColor.b = TexCoordOut.y;
}
