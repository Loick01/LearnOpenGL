#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D objectTexture;

void main()
{
    FragColor = texture(objectTexture, TexCoords);

    // Be sure textures are not already in sRGB space before doing gamma correction manually (below)
    // float gamma = 2.2;
    // FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
    // Usually, diffuse textures are already in sRGB space, while specular maps, normal maps, ... are in linear space
    // If necessary, textures can be loaded with GL_SRGB/GL_SRGB_ALPHA (instead of GL_RGB/GL_RGBA) to automaticaly convert in linear space
}