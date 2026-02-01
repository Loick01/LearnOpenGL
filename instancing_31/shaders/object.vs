#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
// Using instanced arrays
layout (location = 2) in vec2 aOffset;

out vec3 fColor;

// Using uniform
// uniform vec2 offsets[100];

void main()
{
    // Using uniform
    // vec2 offset = offsets[gl_InstanceID]; // gl_InstanceID = 0, 1, 2, 3, etc
    // gl_Position = vec4(aPos + offset, 0.0, 1.0);

    // Using instanced arrays
    // vec2 pos = aPos;
    vec2 pos = aPos * (gl_InstanceID/100.0);
    gl_Position = vec4(pos + aOffset, 0.0, 1.0);

    fColor = aColor;
};