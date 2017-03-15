#version 130
// Spline Fragment Shader

smooth in vec4 VertColor_Frag;
smooth in float Normal_Frag;


out vec4 FragColor;

const float feather_at_percentage = 0.5; //TODO: turn this into a uniform

/*  |___________________|_________+___|
    0                  .6        .9   1
                                  ^         dist_to_line (changes per fragment)
    ^-------------------^                   feather_at_percentage
                        ^--------------^    feather_length

    To determine inverse alpha, find what percentage
    dist_to_line is within the range of feather_length
*/

void main()
{
    // // float dist_to_line = length(Normal_Frag);
    float dist_to_line = abs(Normal_Frag);
    float feather_length = 1.0 - feather_at_percentage;

    float inv_alpha = max(dist_to_line - feather_at_percentage, 0.0); //ignore anything less than feather_at_percentage
    inv_alpha /= feather_length; //divide by range to get actual percentage between feather_at_dist and 1.0

    FragColor = VertColor_Frag;
    FragColor.a = 1.0 - inv_alpha;
}
