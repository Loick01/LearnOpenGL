#version 460 core

struct Material
{
    sampler2D diffuse;
    vec3 specular; // Not a sampler2D
    float shininess;
};

struct Light
{
    // vec3 direction; Unused here
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // Used for attenuation
    float constant;
    float linear;
    float quadratic;
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
uniform Light light;
uniform vec3 viewPos;
uniform vec3 lightPos;

vec3 CalcLight(Light light, vec3 normal, vec3 viewDir, float shadowValue)
{
    // Ambient
    vec3 ambient = light.ambient;

    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos); 
    float diff = max(dot(lightDir, normal), 0.);
    vec3 diffuse = light.diffuse * diff;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);  
    vec3 specular = light.specular * spec * material.specular;

    // Light attenuation
    float distance = length(lightPos - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic * (distance*distance));
    diffuse *= attenuation;
    specular *= attenuation;
    return texture(material.diffuse, fs_in.TexCoords).rgb * (ambient + (1.0-shadowValue)*(diffuse + specular));
}

float GetShadowValue(vec4 fragPosLigthSpace){
    vec3 projCoords = fragPosLigthSpace.xyz / fragPosLigthSpace.w; // [-1, 1]
    projCoords = projCoords * 0.5 + 0.5; // [0, 1]
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    if (currentDepth > 1.0)
        return 0.0;

    // float bias = 0.005;
    vec3 lightDir = normalize(lightPos - fs_in.FragPos); 
    vec3 normal = normalize(fs_in.Normal);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // PCF
    float isShadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1 ; x <= 1 ; x++){
        for (int y = -1 ; y <= 1 ; y++){
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
            isShadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    isShadow /= 9.0;

    // float isShadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return isShadow;
}

void main()
{
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 norm = normalize(fs_in.Normal);
    float shadowValue = GetShadowValue(fs_in.FragPosLightSpace);
    vec3 result = CalcLight(light, norm, viewDir, shadowValue);
    FragColor = vec4(result, 1.0);
}