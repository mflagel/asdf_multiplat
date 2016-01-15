#version 120

varying vec2 TexCoordOut;
varying vec4 ColorOut;

uniform sampler2D TextureMap;

void main(){
	vec4 texcol = texture2D(TextureMap, TexCoordOut);
	gl_FragColor = texcol * ColorOut;
}
