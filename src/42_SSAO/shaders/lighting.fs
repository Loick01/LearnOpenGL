#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

struct Textures // Textures from the G-Buffer
{
    sampler2D position;
    sampler2D normal;
    sampler2D colorSpec;
    sampler2D ssao;
};

struct Light {
    vec3 position;
    vec3 color;
    
    // Used for attenuation
    // constant factor used for attenuation is 1
    float linear;
    float quadratic;
};

uniform Textures gTextures;
uniform Light light;

void main()
{
    vec3 FragPos = texture(gTextures.position, TexCoords).rgb;
    vec3 Normal = texture(gTextures.normal, TexCoords).rgb;
    vec3 Diffuse = texture(gTextures.colorSpec, TexCoords).rgb;
    float AmbientOcclusion = texture(gTextures.ssao, TexCoords).r;

    vec3 ambient = 0.3 * Diffuse * AmbientOcclusion;
    
    vec3 viewDir = normalize(-FragPos); // FragPos is in view space --> viewPos in view space = 0, 0, 0
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.color;

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light.color * spec;

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;

    FragColor = vec4(vec3(ambient + diffuse + specular), 1.);
}