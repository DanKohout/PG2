#version 460 core

// input vertex attributes

in vec3 aPos;   // position: MUST exist
in vec3 aColor; // any additional attributes are optional, any data type, etc.

out vec3 color; // optional output attribute

uniform vec4 uniform_color; // Optional uniform in vertex shader
uniform mat4 model;
//uniform ... texture 

void main()
{
    // Outputs the positions/coordinates of all vertices, MUST WRITE
    gl_Position = vec4(aPos, 1.0f);
    
    color = /*aColor*/vec3(1.0f) * uniform_color.rgb; // copy color to output
}
