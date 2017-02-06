#version 330 core

in vec4 ColorOut;
in vec2 NormalOut;

layout(location = 0) out vec4 FragColor;

const float feather_at_percentage = 0.6;

void main()
{
    float dist_to_line = length(NormalOut);
    float feather_dist = 1.0 - feather_at_percentage;

    FragColor = ColorOut;
    FragColor.a  = 1.0 * float(dist_to_line < feather_at_percentage);


    //  |___________________|_________+___|
    //  0                  .6        .9   1

    //  dist_to_line = .9
    //  feather_dist = .4
    //  


    float feather_alpha = 
    FragColor.a += feather_alpha * float(dist_to_line >= feather_at_percentage);

    FragColor.a = clamp(FragColor.a, 0.0, 1.0);    
}
