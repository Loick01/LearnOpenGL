#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D normalMap;
    vec3 specular; // Not a sampler2D
    float shininess;
};

struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

in VS_OUT {
    // vec3 Normal; // Used only for V1
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

out vec4 FragColor;

uniform Material material;
uniform PointLight light;
uniform vec3 viewPos;
uniform float far_plane;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic * (distance*distance));
    
    // Ambient
    vec3 ambient = light.ambient * texture(material.diffuse, fs_in.TexCoords).rgb;

    // Diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, fs_in.TexCoords).rgb;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * material.specular * spec;

    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

void main()
{
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    
    // V1
    // vec3 norm = normalize(fs_in.Normal);

    // V2 (Normal map)
    // vec3 norm = texture(material.normalMap, fs_in.TexCoords).rgb; // [0, 1]
    // norm = normalize(norm * 2.0 -1.0); // [-1, 1]
    
    // V3 (Normal map in tangent space, use TBN matrix to convert in world space)
    vec3 norm = texture(material.normalMap, fs_in.TexCoords).rgb; // [0, 1]
    norm = norm * 2.0 -1.0; // [-1, 1]
    norm = normalize(fs_in.TBN * norm); // From tangent space to world space
    
    // The tutorial shows a V4 where light position and view position are transformed to tangent space
    
    vec3 result = CalcPointLight(light, norm, fs_in.FragPos, viewDir);
    FragColor = vec4(result, 1.0);
}