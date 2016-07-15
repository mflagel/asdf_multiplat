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

uniform float HEX_HEIGHT = 0.86602540378;
uniform float HEX_HEIGHT_D4 =   0.216506350945;
uniform float HEX_WIDTH = 1.0;
uniform float HEX_WIDTH_D4 = 0.25;

uniform int CHUNK_HEIGHT = 10;

void main(void)
{
    int col_x = gl_InstanceID / CHUNK_HEIGHT;
    int col_y = gl_InstanceID % CHUNK_HEIGHT;

    vec4 pos = VertexPosition;
    pos.x += HEX_WIDTH_D4 * 3 * col_x;
    pos.y += HEX_HEIGHT * col_y;

    pos.y -= float(col_x % 2) * HEX_HEIGHT / 2.0;  //hexagon y offest

	gl_Position = WVP * pos;
	FragTexCoord = vec2(0.0f, 0.0f); //TexCoord;

    vec4 tile_color = TileColors[TileID];
    ColorOut = (VertexColor * 0.000000001) + (tile_color * 0.9);
    ColorOut.a = 1.0;

    ColorOut.a = 1.0;
}
