#version 130
// Hexmap Shader

in vec4 VertexPosition;
in uint TileID;

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
    /// Instanced Rendering is not available below GL 3.1 (glsl 140)
    /*
    /// Position
    int col_x = instanceID / CHUNK_HEIGHT;
    int col_y = instanceID % CHUNK_HEIGHT;

    vec4 pos = VertexPosition;
    pos.x += HEX_WIDTH_D4 * 3 * col_x;
    pos.y += HEX_HEIGHT * col_y;

    pos.y -= float(col_x % 2) * HEX_HEIGHT / 2.0;  //hexagon y offest

    gl_Position = WVP * pos;
    */


	gl_Position = WVP * VertexPosition;


    /// UV Coords
    uint atlas_dim   = uint(1280);
    uint tile_dim    = uint(128);
    uint tile_dim_d2 = uint(64);

    uint num_tiles_width  = atlas_dim / tile_dim;
    uint num_tiles_height = num_tiles_width;

    uint tile_x = (TileID % num_tiles_width) * tile_dim;
    uint tile_y = (TileID / num_tiles_width) * tile_dim;

    // move x/y to the center point of the tile
    tile_x += tile_dim_d2;
    tile_y += tile_dim_d2;


    // hexagon is 1 unit wide, and sin(pi/3) units tall (roughly 0.866)
    // scale it up to the size of a tile texture
    vec2 hex_atlas_coord = vec2(VertexPosition.x * tile_dim, VertexPosition.y * tile_dim);
    hex_atlas_coord += vec2(float(tile_x), float(tile_y));

    //convert to UV
    FragTexCoord = hex_atlas_coord / vec2(float(atlas_dim), float(atlas_dim));


    /// Color
    vec4 tile_color = TileColors[TileID];
    ColorOut = tile_color;
    ColorOut.a = 1.0;
}
