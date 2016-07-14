#version 330 core
// Hexmap Shader

layout(location = 0) in vec4 VertexPosition;
//layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec4 VertexColor;
layout(location = 3) in uint TileID;


smooth out vec2 FragTexCoord;
flat out vec4 ColorOut;

uniform mat4 WVP;
uniform vec4 TileColors[10];

//#define HEX_HEIGHT = 0.86602540378

void main(void)
{
    vec4 pos = VertexPosition;
    pos.y += (0.86602540378) * gl_InstanceID;

	gl_Position = WVP * pos;
	FragTexCoord = vec2(0.0f, 0.0f); //TexCoord;

    vec4 tile_color = TileColors[TileID];
    ColorOut = (VertexColor * 0.5) + (tile_color * 0.5);
    ColorOut = (VertexColor * 0.0000000000000001) + (tile_color * 0.9);
    ColorOut.a = 1.0;

    ColorOut.a = 1.0;
}
