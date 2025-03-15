#version 460 core

in vec3 color; // input from vertex stage of graphics pipeline, automatically interpolated
out vec4 FragColor; // output color of current fragment: MUST be written



uniform vec4 my_color; // Uniform variable

void main()
{
    FragColor =  /*my_color * */vec4(color, 1.0f); // copy RGB color, add Alpha=1.0 (not transparent)
}
