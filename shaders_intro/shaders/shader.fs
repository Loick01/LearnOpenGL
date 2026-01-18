#version 460 core

out vec4 FragColor;
in vec3 vertexColor;
uniform float uniformGreenValue;

void main()
{
    // FragColor = vec4(0., uniformGreenValue, 0., 1.);
    FragColor = vec4(vertexColor, 1.0);
};