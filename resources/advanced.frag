#version 460 core

//struct Material {
//    vec3 ambient;
//    sampler2D diffuseMap;
//    vec3 specular;
//    float shininess;
//};
struct DirLight {
    vec3 direction;

    //vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float exponent;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cosInnerCone;
    float cosOuterCone;
    //vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int on;

    float constant;
    float linear;
    float exponent;
};

struct EmissiveLight {
    vec3 color;
    vec3 position;
    float radius;
};


uniform int spotOn;             // zapnutí / vypnutí
uniform int pointOn;
uniform int dirOn;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

uniform SpotLight spotLight;
uniform PointLight pointLight;//diamond
uniform PointLight pointLight2;//moving cube
uniform DirLight dirLight;

uniform vec3 viewPos;

uniform sampler2D tex0;
uniform vec3 matAmbient;
uniform vec3 matSpecular;
uniform float matShininess;




// diamond
uniform EmissiveLight diamondEmissive;
// cube
uniform EmissiveLight cubeEmissive;
// sun
uniform EmissiveLight sunEmissive;


uniform vec3 ambient;
out vec4 frag_color;

vec3 calcSpotLight();
vec3 calcPointLight(PointLight light);
vec3 calcDirLight();

void main()
{
    // Apply ambient light to base color
    vec3 finalColor = (matAmbient + ambient);

    // Add spotlight only if enabled
    if (spotOn == 1) {
        finalColor += calcSpotLight();
    }

    if (pointOn == 1) {
        finalColor += calcPointLight(pointLight);
        finalColor += calcPointLight(pointLight2);
        // Emissive glow simulation (simple hack)
        float dist = length(diamondEmissive.position - fs_in.FragPos);
        float intensity = clamp(1.0 - dist / diamondEmissive.radius, 0.0, 1.0);
        finalColor += diamondEmissive.color * intensity;
        // moving cube
        dist = length(cubeEmissive.position - fs_in.FragPos);
        intensity = clamp(1.0 - dist / cubeEmissive.radius, 0.0, 1.0);
        finalColor += cubeEmissive.color * intensity;
    }

    if (dirOn == 1)
        // Add directional light (sun)
        finalColor += calcDirLight();

    //the sun object can glow even underneath the ground - it doesnt matter
    float distSun = length(sunEmissive.position - fs_in.FragPos);
    float sunGlow = 1.0 / (0.01 + pow(distSun / sunEmissive.radius, 2.0));
    finalColor += sunEmissive.color * sunGlow;
    //finalColor += sunEmissiveColor * 1.0;

    //adding textures and putting it in the frag_color
    frag_color = vec4(finalColor, 1.0) * texture(tex0, fs_in.TexCoord);
}

//--------------------------------------------------------------
// Calculate the spotlight diffuse and specular components
//--------------------------------------------------------------
vec3 calcSpotLight()
{
    vec3 lightDir = normalize(spotLight.position - fs_in.FragPos);
    vec3 spotDir  = normalize(spotLight.direction);

    float cosDir = dot(-lightDir, spotDir);
    float spotIntensity = smoothstep(spotLight.cosOuterCone, spotLight.cosInnerCone, cosDir);

    vec3 normal = normalize(fs_in.Normal);
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * NdotL;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float NDotH = max(dot(normal, halfDir), 0.0);
    vec3 specular = spotLight.specular * matSpecular * pow(NDotH, matShininess);

    float distance = length(spotLight.position - fs_in.FragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.exponent * (distance * distance));

    return (diffuse + specular) * attenuation * spotIntensity;
}


vec3 calcPointLight(PointLight light)
{
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 fragPos = fs_in.FragPos;
    
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.exponent * (distance * distance));

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = light.specular * spec * matSpecular;

    return attenuation * (diffuse + specular);
}




vec3 calcDirLight()
{
    vec3 lightDir = normalize(-dirLight.direction);  // Light direction points *toward* surface

    // Diffuse
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * diff;

    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = dirLight.specular * spec * matSpecular;

    // No attenuation for directional light
    return diffuse + specular;
}