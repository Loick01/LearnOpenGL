#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

// Interface block
out VERTEX_OUT {
    vec2 TexCoords;
} vertex_out;

uniform mat4 model;

// Uniform block
// std140, shared, packed
// OpenGl 4.2+, we can specify the binding point here instead of using glGetUniformBlockIndex
layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertex_out.TexCoords = aTexCoords;
    // gl_PointSize = gl_Position.z;
    // gl_VertexID --> when doing indexed rendering (using glDrawElements), holds the current index of drawed vertex. This is what I used for my voxel engine

};