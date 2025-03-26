#version 460 core

in VS_OUT
{
    vec2 texcoord;
} fs_in;
uniform sampler2D tex0; // texture unit from C++

in vec3 color; // input from vertex stage of graphics pipeline, automatically interpolated
out vec4 FragColor; // output color of current fragment: MUST be written



uniform vec4 my_color; // Uniform variable

void main()
{
    //FragColor =  /*my_color*/ vec4(color, 1.0f); // copy RGB color, add Alpha=1.0 (not transparent)
    
    // use only texture
    FragColor = texture(tex0, fs_in.texcoord);

    // combine with material
    //FragColor = my_color * texture(tex0, fs_in.texcoord);
}
