uniform mat4 WVP;
uniform mat3 NormalMatrix;
uniform vec4 LightPos;

void main(void)
{			
	//calculate normal	
	vec3 normal = normalize(NormalMatrix * gl_Normal);		
	
	//calculate diffuse	
	float diffuse = max(dot(normal, LightPos), 0.0);
	
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position = WVP * gl_Vertex;
    gl_FrontColor = gl_Color * diffuse; //mult final color by diffuse value
	gl_FrontColor.w = 1;
}
