#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out; // Output 6 triangles (18 vertices)

uniform mat4 lightSpaceMatrices[6];

out vec4 FragPos;

void main(){
    for (int face = 0 ; face < 6 ; face++){
        gl_Layer = face;
        for (int i = 0 ; i < 3 ; i++){
            FragPos = gl_in[i].gl_Position;
            gl_Position = lightSpaceMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
