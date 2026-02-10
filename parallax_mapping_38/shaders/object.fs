#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D normalMap;
    sampler2D displacementMap;
    vec3 specular; // Not a sampler2D
    float shininess;
};

struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    // I have removed the attenuation
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

out vec4 FragColor;

uniform Material material;
uniform PointLight light;
uniform vec3 viewPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{   
    vec3 color = texture(material.diffuse, fs_in.TexCoords).rgb;

    // Ambient
    vec3 ambient = light.ambient;

    // Diffuse
    vec3 lightDir = normalize(light.position - fragPos);
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
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    
    // Normal map in tangent space, use TBN matrix to convert in world space
    vec3 norm = texture(material.normalMap, fs_in.TexCoords).rgb; // [0, 1]
    norm = norm * 2.0 -1.0; // [-1, 1]
    norm = normalize(fs_in.TBN * norm); // From tangent space to world space
    
    vec3 result = CalcPointLight(light, norm, fs_in.FragPos, viewDir);
    FragColor = vec4(result, 1.0);
}