#version 460 core

in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec2 texcoord;
} fs_in;

uniform sampler2D tex0; // texture unit from C++
uniform vec4 u_diffuse_color = vec4(1.0f);

in vec3 color; // input from vertex stage of graphics pipeline, automatically interpolated
out vec4 FragColor; // output color of current fragment: MUST be written

// Material properties
uniform vec3 ambient_intensity, diffuse_intensity, specular_intensity;
uniform vec3 ambient_material, diffuse_material, specular_material;
uniform float specular_shinines;


//uniform vec4 my_color; // Uniform variable


// Spotlight uniforms (in view space)
uniform vec3 spot_position;    // flashlight origin (usually camera position in view space)
uniform vec3 spot_direction;   // flashlight direction (usually -camera forward)
uniform float spot_cutoff;     // cosine of inner angle
uniform float spot_outer_cutoff; // cosine of outer angle (for soft edge)

void main()
{
    //Normalize the incoming N, L and V vectors
    vec3 N = normalize(fs_in.N);
    vec3 L = normalize(fs_in.L);
    vec3 V = normalize(fs_in.V);

    // Calculate R by reflecting -L around the plane defined by N
    vec3 R = reflect(-L, N);

    // Calculate the ambient, diffuse and specular contributions
    vec3 ambient = ambient_material * ambient_intensity;
    vec3 diffuse = max(dot(N, L), 0.0) * diffuse_material * diffuse_intensity;
    vec3 specular = pow(max(dot(R, V), 0.0), specular_shinines) * specular_material * specular_intensity;

    FragColor = u_diffuse_color * vec4((ambient + diffuse) + specular, 1.0) * texture(tex0, fs_in.texcoord);

    // + transparent objects
    //FragColor = u_diffuse_color * texture(tex0, fs_in.texcoord);
}
