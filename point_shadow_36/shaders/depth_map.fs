#version 460 core

in vec4 FragPos; // Position of the fragment in one of the 6 light spaces

uniform vec3 lightPos;
uniform float far_plane;

void main() {
    float distanceToLight = length(FragPos.xyz - lightPos);
    distanceToLight /= far_plane; // Map the value to [0, 1]
    gl_FragDepth = distanceToLight;
}