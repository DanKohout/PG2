#version 460 core

// input vertex attributes

in vec3 aPos;   // position: MUST exist
//in vec3 aColor; // any additional attributes are optional, any data type, etc.
in vec3 aNormal; //attribute normal
in vec2 aTexCoords; //attribute_texCoords

out vec3 color; // optional output attribute
out vec3 normal;
out vec2 texcoord;

// Light properties
uniform vec3 light_position;

out VS_OUT
{
    vec3 N;//normal
    vec3 L;//light vector
    vec3 V;//view vector?
    vec2 texcoord;
} vs_out;



uniform sampler2D tex0; // texture unit from C++

uniform vec4 uniform_color; // Optional uniform in vertex shader
//uniform ... texture 

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
    //texcoord = aTexCoords;//TODO
    
    
    // Create Model-View matrix
    mat4 mv_m = uV_m * uM_m;
    // Calculate view-space coordinate - in P point 
    // we are computing the color
    vec4 P = mv_m * vec4(aPos, 1.0f);

    // Calculate normal in view space
    vs_out.N = mat3(mv_m) * aNormal;
     // Calculate view-space light vector
    vs_out.L = vec3(uV_m*vec4(light_position, 1.0f)) - P.xyz;
    // Calculate view vector (negative of the view-space position)
    vs_out.V = -P.xyz;

    //gl_Position = uP_m * P;
    //gl_Position = uP_m * uV_m * uM_m * vec4(aPos, 1.0f);
    
    vs_out.texcoord = aTexCoords;

    color = /*aColor*/vec3(1.0f) * uniform_color.rgb; // copy color to output
}
