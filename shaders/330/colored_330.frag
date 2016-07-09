#version 330 core

in vec4 ColorOut;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = ColorOut;
	FragColor.a = 1; //in case I'm dumb and forget to assign Color
}
