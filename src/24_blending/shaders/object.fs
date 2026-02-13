#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D objectTexture;

void main()
{
    // For ,grassTexture discard the current fragment when alpha is < a threshold
    // I also used discard for windowTexture because the corners are 100% transparent in the png 
    vec4 texColor = texture(objectTexture, TexCoords);
    if (texColor.a < 0.1) discard;
    FragColor = texColor;
}