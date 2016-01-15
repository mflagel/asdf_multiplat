uniform vec4 AmbientColor;
uniform float ambientStrength;

void main(void)
{
	//gl_FragColor = gl_Color * vec4(ambientStrength, ambientStrength, ambientStrength, 1.0f);
	gl_FragColor = gl_Color * AmbientColor;
}
