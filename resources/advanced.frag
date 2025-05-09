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
    // Ambient lighting
    vec3 ambient = spotLight.ambient * matAmbient;

    vec3 spotColor = vec3(0.1);

    // If the light is on, compute spotlight contribution
    //if (spot_on == 1)
    //if (spotLight.on == 1)
    if (spotOn == 1){
        spotColor = calcSpotLight();
    }
    //frag_color = vec4(1, 0.0, 0.0, 1.0);
    frag_color = vec4(ambient + spotColor, 1.0) * texture(tex0, fs_in.TexCoord);
}

//--------------------------------------------------------------
// Calculate the spotlight diffuse and specular components
//--------------------------------------------------------------
vec3 calcSpotLight()
{
    vec3 lightDir = normalize(spotLight.position - fs_in.FragPos);
    vec3 spotDir  = normalize(spotLight.direction);

    float cosDir = dot(-lightDir, spotDir);  // cone angle
    float spotIntensity = smoothstep(spotLight.cosOuterCone, spotLight.cosInnerCone, cosDir);

    vec3 normal = normalize(fs_in.Normal);
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * NdotL * texture(tex0, fs_in.TexCoord).rgb;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float NDotH = max(dot(normal, halfDir), 0.0);
    vec3 specular = spotLight.specular * matSpecular * pow(NDotH, matShininess);

    float distance = length(spotLight.position - fs_in.FragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.exponent * (distance * distance));

    diffuse *= attenuation * spotIntensity;
    specular *= attenuation * spotIntensity;

    return diffuse + specular;
}