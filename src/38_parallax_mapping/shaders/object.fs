#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D normalMap;
    sampler2D heightMap;
    vec3 specular; // Not a sampler2D
    float shininess;
};

struct PointLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    // I have removed the attenuation
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

out vec4 FragColor;

uniform Material material;
uniform PointLight light;
uniform vec3 viewPos;
uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir){
    float height = texture(material.heightMap, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height*heightScale);
    return texCoords - p;
}

vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir){
    // const float numLayers = 10.0;
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));

    float layerDepth = 1.0/numLayers;
    float currentLayerDepth = 0.0;
    vec2 p = viewDir.xy * heightScale;
    vec2 deltaTexCoords = p / numLayers;
    
    vec2 currentTexCoords = texCoords;
    float currentHeightMapValue = texture(material.heightMap, currentTexCoords).r;
    while(currentLayerDepth < currentHeightMapValue){
        currentTexCoords -= deltaTexCoords;
        currentHeightMapValue = texture(material.heightMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }
    vec2 previousTexCoords = currentTexCoords + deltaTexCoords;
    float afterDpeth = currentHeightMapValue - currentLayerDepth;
    float beforeDepth = texture(material.heightMap, previousTexCoords).r - currentLayerDepth + layerDepth;
    float weight = afterDpeth / (afterDpeth-beforeDepth);
    vec2 resultTexCoords = previousTexCoords * weight + currentTexCoords * (1.0 - weight);
    return resultTexCoords;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{   
    vec3 color = texture(material.diffuse, fs_in.TexCoords).rgb;

    // Ambient
    vec3 ambient = light.ambient;

    // Diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.);
    vec3 diffuse = light.diffuse * diff;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * material.specular * spec;

    return color * (ambient + diffuse) + specular;
}

void main()
{
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    
    // vec2 texCoords = fs_in.TexCoords; // Without parallax mapping
    // vec2 texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);
    vec2 texCoords = SteepParallaxMapping(fs_in.TexCoords, viewDir);
    if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0){
        discard;
    }
    
    // Normal map in tangent space, use TBN matrix to convert in world space
    vec3 norm = texture(material.normalMap, texCoords).rgb; // [0, 1] (already in tangent space)
    norm = norm * 2.0 -1.0; // [-1, 1]
    
    vec3 result = CalcPointLight(light, norm, fs_in.TangentFragPos, viewDir);
    FragColor = vec4(result, 1.0);
}