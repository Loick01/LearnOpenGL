#version 460 core

struct Material
{
    sampler2D diffuse;
    vec3 specular; // Not a sampler2D
    float shininess;
};

struct DirLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoords;
    vec4 FragPosLightSpace; 
} fs_in;

out vec4 FragColor;

uniform Material material;
uniform sampler2D shadowMap;
uniform DirLight dirLight;
uniform vec3 viewPos;
uniform vec3 lightPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadowValue)
{
    // Ambient
    vec3 ambient = light.ambient * texture(material.diffuse, fs_in.TexCoords).rgb;

    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos); 
    float diff = max(dot(lightDir, normal), 0.);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, fs_in.TexCoords).rgb;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    return ambient + (1.0-shadowValue)*(diffuse + specular);
}

float GetShadowValue(vec4 fragPosLigthSpace){
    vec3 projCoords = fragPosLigthSpace.xyz / fragPosLigthSpace.w; // [-1, 1]
    projCoords = projCoords * 0.5 + 0.5; // [0, 1]
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float isShadow = currentDepth > closestDepth ? 1.0 : 0.0;
    return isShadow;
}

void main()
{
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 norm = normalize(fs_in.Normal);
    float shadowValue = GetShadowValue(fs_in.FragPosLightSpace);
    vec3 result = CalcDirLight(dirLight, norm, viewDir, shadowValue);
    FragColor = vec4(result, 1.0);
}