#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
// Bitangent is cross(normal, tangent)

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); // World space position of the fragment
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.TexCoords = aTexCoords;
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 B = cross(N, T);
    vs_out.TBN = mat3(T, B, N);
};