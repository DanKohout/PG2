#version 460 core

in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec2 texcoord;
    vec3 frag_pos_view; // <- view-space pozice pro spotlight
} fs_in;

uniform sampler2D tex0;
uniform vec4 u_diffuse_color = vec4(1.0f);

// === Materiál + svìtlo ===
uniform vec3 ambient_intensity, diffuse_intensity, specular_intensity;
uniform vec3 ambient_material, diffuse_material, specular_material;
uniform float specular_shinines;

// === Spotlight (baterka) ===
uniform vec3 spot_position;      // pozice reflektoru
uniform vec3 spot_direction;     // smìr reflektoru
uniform float spot_cutoff;       // vnitøní úhel (cos)
uniform float spot_outer_cutoff; // vnìjší úhel (cos) – pro hladký okraj
uniform int spot_on;             // zapnutí / vypnutí

out vec4 FragColor;

uniform vec3 camera_position;




vec3 calcSpotlight(vec3 fragPos, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(spot_position - fragPos);
    float distance = length(spot_position - fragPos);

    // Spotlight angle-based intensity
    float cosAngle = dot(-lightDir, normalize(spot_direction));
    float epsilon = spot_cutoff - spot_outer_cutoff;
    float spotIntensity = clamp((cosAngle - spot_outer_cutoff) / epsilon, 0.0, 1.0);

    // If outside of the outer cone, no light
    if (spotIntensity <= 0.0)
        return vec3(0.0);

    // Blinn-Phong lighting
    vec3 halfVec = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfVec), 0.0), specular_shinines);

    vec3 ambientLight = ambient_material * ambient_intensity * 0.1 * spotIntensity;
    vec3 diffuseLight = diffuse_material * diffuse_intensity * diff * spotIntensity;
    vec3 specularLight = specular_material * specular_intensity * spec * spotIntensity;

    return ambientLight + diffuseLight + specularLight;
}



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

    // === Spotlight (baterka) ===
    // === Spotlight (baterka) ===
    if (spot_on == 1)
    {
        vec3 spotLightColor = calcSpotlight(fs_in.frag_pos_view, N, V);
        ambient  += spotLightColor; // You can also split these into ambient/diffuse/spec if you prefer
    }


    // Výsledná barva + textura
    FragColor = u_diffuse_color * vec4(ambient + diffuse + specular, 1.0) * texture(tex0, fs_in.texcoord);

    // + transparent objects
    //FragColor = u_diffuse_color * texture(tex0, fs_in.texcoord);
}

