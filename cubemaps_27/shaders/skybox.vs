#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    TexCoords = aPos; // Because the cube is centered, its position vector is also a direction vector from the origin
};