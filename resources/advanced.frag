#version 460 core

//struct Material {
//    vec3 ambient;
//    sampler2D diffuseMap;
//    vec3 specular;
//    float shininess;
//};
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

uniform int spotOn;             // zapnutí / vypnutí
uniform int pointOn;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

uniform SpotLight spotLight;
uniform PointLight pointLight;

uniform vec3 viewPos;

uniform sampler2D tex0;
uniform vec3 matAmbient;
uniform vec3 matSpecular;
uniform float matShininess;


uniform vec3 emissiveColor;
uniform vec3 emissivePosition;
uniform float emissiveRadius;

uniform vec3 ambient;
out vec4 frag_color;

vec3 calcSpotLight();
vec3 calcPointLight();

void main()
{
    // Apply ambient light to base color
    vec3 finalColor = (matAmbient + ambient);

    // Add spotlight only if enabled
    if (spotOn == 1) {
        finalColor += calcSpotLight();
    }

    if (pointOn == 1) {
        finalColor += calcPointLight();

        // Emissive glow simulation (simple hack)
        float dist = length(emissivePosition - fs_in.FragPos);
        float intensity = clamp(1.0 - dist / emissiveRadius, 0.0, 1.0);
        finalColor += emissiveColor * intensity;
    }

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


vec3 calcPointLight()
{
    vec3 lightDir = normalize(pointLight.position - fs_in.FragPos);
    float distance = length(pointLight.position - fs_in.FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.exponent * (distance * distance));

    // Diffuse
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuse * diff;

    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = pointLight.specular * spec * matSpecular;

    return attenuation * (diffuse + specular);
}