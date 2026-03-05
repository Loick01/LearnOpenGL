#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

struct Textures // Textures from the G-Buffer
{
    sampler2D position;
    sampler2D normal;
    sampler2D colorSpec;
};

uniform Textures gTextures;

void main()
{
    vec3 FragPos = texture(gTextures.position, TexCoords).rgb;
    vec3 Normal = texture(gTextures.normal, TexCoords).rgb;
    vec3 Albedo = texture(gTextures.colorSpec, TexCoords).rgb;
    float Specular = texture(gTextures.colorSpec, TexCoords).a;

    FragColor = vec4(FragPos, 1.);
}