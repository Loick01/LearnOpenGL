#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

struct Textures // Textures from the G-Buffer
{
    sampler2D position;
    sampler2D normal;
    sampler2D colorSpec;
};

struct Light {
    vec3 position;
    vec3 color;
    
    // Used for attenuation
    // constant factor used for attenuation is 1
    float linear;
    float quadratic;

    float radius;
};

const int NR_LIGHTS = 32;
uniform Textures gTextures;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{
    vec3 FragPos = texture(gTextures.position, TexCoords).rgb;
    vec3 Normal = texture(gTextures.normal, TexCoords).rgb;
    vec3 Diffuse = texture(gTextures.colorSpec, TexCoords).rgb;
    float Specular = texture(gTextures.colorSpec, TexCoords).a;

    // FragColor = vec4(FragPos);

    // Ambient
    vec3 lighting = Diffuse * 0.1;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    for (int i = 0 ; i < NR_LIGHTS ; i++){
        float distance = length(lights[i].position - FragPos);
        if (distance < lights[i].radius) { // Not efficient
            // Diffuse
            vec3 lightDir = normalize(lights[i].position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].color;
            
            // Specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = lights[i].color * spec * Specular;
            
            // Attenuation
            float attenuation = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }   
    }

    FragColor = vec4(lighting, 1.);
}