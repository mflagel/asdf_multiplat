#version 130

smooth in vec2 FragTexCoord;

/*layout(location = 0) */out vec4 FragColor;

uniform sampler2D TextureMap;
uniform vec4 Color;
 
void main()
{
	vec4 texcol = texture2D(TextureMap, FragTexCoord);

    vec4 color = clamp(Color, vec4(0.0), vec4(1.0));
    vec4 overflow = Color - vec4(1.0);

    FragColor = texcol * color;
    FragColor += overflow;
}
