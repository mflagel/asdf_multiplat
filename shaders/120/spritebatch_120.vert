#version 120

attribute vec2 Position;
attribute vec2 TexCoord;
attribute vec4 VertColor;

varying vec2 TexCoordOut;
varying vec4 ColorOut;

uniform mat4 WVP;


void main(){
	vec4 pos = vec4(1);
	pos.xy = Position;
	gl_Position = WVP * pos;
	TexCoordOut = TexCoord;
	ColorOut = VertColor;
}
