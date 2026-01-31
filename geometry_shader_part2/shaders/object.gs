#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 TexCoords;
} gs_in[];

out vec2 fTexCoords;

void main() {
    for (int i = 0 ; i < 3 ; i++){
        fTexCoords = gs_in[i].TexCoords;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}