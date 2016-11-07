#version 330 core
// Hexmap Shader

smooth in vec2 FragTexCoord;
flat in vec4 ColorOut;

layout(location = 0) out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D TextureMap;
uniform bool ApplyTexture;
 
void main() {

    FragColor = ColorOut;

    //TODO: use a different frag shader for cases where no texture is needed
    if(ApplyTexture)
    {
        vec4 texcol = texture2D(TextureMap, FragTexCoord);
        FragColor = texcol;
        FragColor.a = 1.0f;
    }

    FragColor *= Color;
}
