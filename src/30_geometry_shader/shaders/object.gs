#version 460 core

layout (points) in;
// layout (points, max_vertices = 1) out; // Pass-through shader
// layout (line_strip, max_vertices = 2) out; // Draw lines
layout (triangle_strip, max_vertices = 5) out; // Draw houses

// gl_in[0] --> gl_Position
// gl_in[1] --> gl_PointSize
// gl_in[2] --> gl_ClipDistance

in VS_OUT {
    vec3 Color;
} gs_in[]; // OpenGL requires geometry inputs to be arrays

out vec3 fColor;

void buildHouse(vec4 position){
    // Vertex color must be defined before using EmitVertex
    fColor = gs_in[0].Color; // gs_in is an array + gs_in[0] because there is only one input vertex

    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);
    EmitVertex(); // When a vertex is emitted, the last stored value in fColor will be used for as color for this vertex in the fragment shader
    gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0);
    fColor = vec3(1.0);
    EmitVertex();
    EndPrimitive();
}

void main() {

    // Pass-through gshader
    // gl_Position = gl_in[0].gl_Position;
    // EmitVertex();
    // EndPrimitive();
    
    // Draw lines
    // gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
    // EmitVertex();
    // gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
    // EmitVertex();
    // EndPrimitive();

    // Draw houses
    buildHouse(gl_in[0].gl_Position);
}