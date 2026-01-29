#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
    // gl_PointSize = gl_Position.z;
    // gl_VertexID --> when doing indexed rendering (using glDrawElements), holds the current index of drawed vertex. This is what I used for my voxel engine

};