#version 460 core

layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Maximum amount of data allowed for a vertex attribute is equal to vec4
layout (location = 3) in mat4 instanceModel;
// Because a mat4 is assigned to location 3, columns of the matrix will have vertex location 3, 4, 5 and 6
out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * instanceModel * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
};