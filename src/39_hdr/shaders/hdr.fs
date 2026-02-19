#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D hdrBuffer;
uniform float exposure;

void main()
{
    // vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    // FragColor = vec4(hdrColor, 1.0);

    // Reinhard tone mapping
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    // vec3 mapped = hdrColor / (hdrColor + vec3(1.));
    vec3 mapped = vec3(1.0) - exp(-hdrColor*exposure);
    mapped = pow(mapped, vec3(1./gamma));
    FragColor = vec4(mapped, 1.);
}