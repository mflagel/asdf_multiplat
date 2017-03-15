#version 130
// Spline Vertex Shader

in vec2 VertexPosition;
in vec2 VertexExtrusion;
in float SplineNormal; //used for interpolation to get dist-to-line in frag shader
in vec4 VertexColor;


//smooth out vec2 FragTexCoord;
smooth out vec4 VertColor_Frag;
smooth out float Normal_Frag;

uniform mat4 WVP;

const float LineThickness = 0.1;  //todo: replace constant with thickness

void main(void)
{
    /// Position
    vec4 pos = vec4(VertexPosition + VertexExtrusion * LineThickness, 0.0, 1.0);
    gl_Position = WVP * pos;

    VertColor_Frag = VertexColor;
    Normal_Frag = SplineNormal;
}
