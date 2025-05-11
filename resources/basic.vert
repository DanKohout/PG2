#version 460 core

// input vertex attributes

layout(location = 0) in vec3 aPos;   // position: MUST exist
//in vec3 aColor; // any additional attributes are optional, any data type, etc.
layout(location = 1) in vec3 aNormal; //attribute normal
layout(location = 2) in vec2 aTexCoords; //attribute_texCoords

//out vec3 color; // optional output attribute
//out vec3 normal;
//out vec2 texcoord;

// Light properties
//uniform vec3 light_direction; 


uniform mat4 uM_m = mat4(1.0);//uniform mat4 model;
uniform mat4 uV_m = mat4(1.0);//uniform mat4 view;
uniform mat4 uP_m = mat4(1.0);//uniform mat4 projection;




//out VS_OUT
//{
//    vec3 N; // normal
//    vec3 L; // light vector
//    //vec3 Lspot; // light vector
//    vec3 V; // view vector
//    vec2 texcoord;
//    vec3 frag_pos_view; // <- tohle je důležité
//} vs_out;



out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} vs_out;


//uniform sampler2D tex0; // texture unit from C++

//uniform vec4 uniform_color; // Optional uniform in vertex shader
//uniform ... texture 

//uniform mat4 uP_m = mat4(1.0); // Projection transformation matrix
//uniform mat4 uM_m = mat4(1.0);
//uniform mat4 uV_m = mat4(1.0); // View (camera) transformation matrix

uniform float tex_scale = 1.0f; // škálování texturovacích souřadnic (opakování textury)

void main()
{
    // Outputs the positions/coordinates of all vertices
//    mat4 mv_m = uV_m * uM_m;
//    vec4 P = mv_m * vec4(aPos, 1.0f);
    
//    vs_out.frag_pos_view = P.xyz;


    // Calculate normal in view space
//    vs_out.N = mat3(mv_m) * aNormal;

    // Calculate view-space light vector
//   vs_out.L = normalize(mat3(uV_m) * -light_direction); // světlo má směr, ne pozici
    //vs_out.Lspot = uV_m * (-light_direction);
    // Calculate view vector (negative of the view-space position)
//    vs_out.V = -P.xyz;

//    vs_out.texcoord = aTexCoords * tex_scale; // násobení opakování textury

//    color = /*aColor*/vec3(1.0f) * uniform_color.rgb; // copy color to output

    // Final transformed position
//    gl_Position = uP_m * P;


    vec4 worldPos = uM_m * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz;
    vs_out.Normal = mat3(transpose(inverse(uM_m))) * aNormal;
    vs_out.TexCoord = aTexCoords * tex_scale; // násobení opakování textury;

    gl_Position = uP_m * uV_m * worldPos;
}
