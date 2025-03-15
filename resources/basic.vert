#version 460 core

// input vertex attributes

in vec3 aPos;   // position: MUST exist
in vec3 aColor; // any additional attributes are optional, any data type, etc.
in vec3 aNormal; //attribute normal
in vec2 aTexCoords; //attribute_texCoords
out vec3 color; // optional output attribute
out vec3 normal;
out vec2 texcoord;

uniform vec4 uniform_color; // Optional uniform in vertex shader
//uniform ... texture 

//uniform mat4 model;
//uniform mat4 view;          // View (camera) transformation matrix
//uniform mat4 projection;    // Projection transformation matrix



uniform mat4 uP_m = mat4(1.0); // Projection transformation matrix
uniform mat4 uM_m = mat4(1.0);
uniform mat4 uV_m = mat4(1.0); // View (camera) transformation matrix



void main()
{
    // Outputs the positions/coordinates of all vertices, MUST WRITE
    //gl_Position = vec4(aPos, 1.0f);
    // Outputs the positions/coordinates of all vertices
    gl_Position = uP_m * uV_m * uM_m * vec4(aPos, 1.0f);
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
    normal = aNormal;//TODO
    //normal = mat3(transpose(inverse(modelM))) * aNormal; // Normal correction for non-uniform scaling
    texcoord = aTexCoords;//TODO

    color = /*aColor*/vec3(1.0f) * uniform_color.rgb; // copy color to output
}
