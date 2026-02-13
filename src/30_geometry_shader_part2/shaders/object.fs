#version 460 core

in vec2 fTexCoords;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main()
{
    vec3 result = texture(texture_diffuse1, fTexCoords).rgb;
    FragColor = vec4(result, 1.0);
}