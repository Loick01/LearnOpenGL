#version 460 core

in VERTEX_OUT {
    vec2 TexCoords;
} fragment_in;

out vec4 FragColor;

uniform sampler2D objectTexture;

void main()
{
    FragColor = texture(objectTexture, fragment_in.TexCoords);
}