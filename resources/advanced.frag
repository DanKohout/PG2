#version 460 core

//struct Material {
//    vec3 ambient;
//    sampler2D diffuseMap;
//    vec3 specular;
//    float shininess;
//};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cosInnerCone;
    float cosOuterCone;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int on;

    float constant;
    float linear;
    float exponent;
};

uniform int spotOn;             // zapnutí / vypnutí

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

uniform SpotLight spotLight;
//uniform Material material;
uniform vec3 viewPos;

uniform sampler2D tex0;
uniform vec3 matAmbient;
uniform vec3 matSpecular;
uniform float matShininess;


out vec4 frag_color;

vec3 calcSpotLight();

void main()
{
    // Apply ambient light to base color
    vec3 finalColor = (matAmbient + spotLight.ambient);

    // Add spotlight only if enabled
    if (spotOn == 1) {
        finalColor += calcSpotLight();
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