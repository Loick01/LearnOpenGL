#version 460 core

// MRT
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform Material material;

void main()
{
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gColorSpec.rgb = texture(material.texture_diffuse1, TexCoords).rgb;
    gColorSpec.a = texture(material.texture_specular1, TexCoords).r;
}