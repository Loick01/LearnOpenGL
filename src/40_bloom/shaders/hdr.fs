#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D scene;
uniform sampler2D blurScene; 
uniform float exposure;

void main()
{
    // Reinhard tone mapping
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(blurScene, TexCoords).rgb;
    hdrColor += bloomColor;
    // vec3 mapped = hdrColor / (hdrColor + vec3(1.));
    vec3 mapped = vec3(1.0) - exp(-hdrColor*exposure);
    mapped = pow(mapped, vec3(1./gamma));
    FragColor = vec4(mapped, 1.);
}