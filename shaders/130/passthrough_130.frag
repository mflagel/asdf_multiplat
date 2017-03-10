#version 130

out vec4 FragColor;

uniform vec4 Color;

void main()
{
	FragColor = Color;
	//FragColor.a = 1; //in case I'm dumb and forget to assign Color
}
