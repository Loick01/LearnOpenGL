#version 460 core

struct Material
{
    sampler2D diffuse;
};

struct PointLight
{
    vec3 Position;
    vec3 Color;
    // vec3 ambient;
    // vec3 diffuse;
    // vec3 specular;
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

out vec4 FragColor;

uniform Material material;
uniform PointLight lights[4];
uniform vec3 viewPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 color)
{   
    // Diffuse
    vec3 lightDir = normalize(light.Position - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.);
    vec3 diffuse = light.Color * diff * color;
    float distance = length(fs_in.FragPos - light.Position);
    diffuse *= 1./(distance*distance);
    return diffuse;
}

void main()
{   
    vec3 normal = normalize(fs_in.Normal);
    vec3 color = texture(material.diffuse, fs_in.TexCoords).rgb;
    vec3 ambient = 0.1 * color;

    vec3 lightning = vec3(0.);
    for (int i = 0 ; i < 4 ; i++){
        lightning += CalcPointLight(lights[i], normal, color);
    }
    FragColor = vec4(ambient + lightning, 1.0);
}