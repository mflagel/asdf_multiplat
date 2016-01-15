#version 330 core

uniform sampler2D TextureMap;
uniform vec4 Color = vec4(1,1,1,1);

out vec4 FragColor;
 
void main() {
	//must glEnable(GL_POINT_SPRITE) for gl_PointCoord
	FragColor = texture2D(TextureMap, gl_PointCoord) * Color;
	//FragColor = vec4(1,1,1,1);
	//FragColor.rg = gl_PointCoord.st;
}
