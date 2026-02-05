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
} fs_in;

out vec4 FragColor;

uniform Material material;
uniform samplerCube shadowMap;
uniform Light light;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform float far_plane;

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

float GetShadowValue(vec3 fragPos){
    vec3 fragToLight = fragPos - lightPos; // Don't need to be normalized
    float closestDepth = texture(shadowMap, fragToLight).r; // [0, 1]
    closestDepth *= far_plane; // [0, far_plane]
    float currentDepth = length(fragToLight);

    // if (currentDepth > 1.0)
    //     return 0.0;

    float bias = 0.05;

    float isShadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return isShadow;
}

void main()
{
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 norm = normalize(fs_in.Normal);
    float shadowValue = GetShadowValue(fs_in.FragPos);
    vec3 result = CalcLight(light, norm, viewDir, shadowValue);
    FragColor = vec4(result, 1.0);
}