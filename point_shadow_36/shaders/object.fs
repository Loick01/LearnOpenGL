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

vec3 sampleOffsetDirections[20] = vec3[]
(
    vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
    vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
    vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float GetShadowValue(vec3 fragPos){
    vec3 fragToLight = fragPos - lightPos; // Don't need to be normalized
    float currentDepth = length(fragToLight);

    // float bias = 0.05;

    vec3 lightDir = normalize(lightPos - fragPos); 
    vec3 normal = normalize(fs_in.Normal);
    float bias = max(0.15 * (1.0 - dot(normal, lightDir)), 0.15);

    // float closestDepth = texture(shadowMap, fragToLight).r; // [0, 1]
    // closestDepth *= far_plane; // [0, far_plane]
    // float isShadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    // // PCF
    // float isShadow = 0.0;
    // float samples = 4.0;
    // float offset = 0.1;
    // for (float x = -offset ; x <= offset ; x+=offset/(samples*0.5)){
    //     for (float y = -offset ; y <= offset ; y+=offset/(samples*0.5)){
    //         for (float z = -offset ; z <= offset ; z+=offset/(samples*0.5)){
    //             float closestDepth = texture(shadowMap, fragToLight + vec3(x,y,z)).r; // [0, 1]
    //             closestDepth *= far_plane; // [0, far_plane]
    //             if (currentDepth - bias > closestDepth)
    //                 isShadow += 1.0;
    //         }
    //     }
    // }
    // isShadow /= (samples*samples*samples);

    float isShadow = 0.0;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    // float diskRadius = 0.05;
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i){
        float closestDepth = texture(shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane; // [0, far_plane]
        if(currentDepth - bias > closestDepth)
            isShadow += 1.0;
    }
    isShadow /= float(samples);
    
    return isShadow;
}

void main()
{
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 norm = normalize(fs_in.Normal);
    float shadowValue = GetShadowValue(fs_in.FragPos);
    vec3 result = CalcLight(light, norm, viewDir, shadowValue);
    FragColor = vec4(result, 1.0);

    // DEBUG SHADOW CUBEMAP 
    // vec3 fragToLight = fs_in.FragPos - lightPos; // Don't need to be normalized
    // float closestDepth = texture(shadowMap, fragToLight).r; // [0, 1]
    // FragColor = vec4(vec3(closestDepth), 1.0);
}