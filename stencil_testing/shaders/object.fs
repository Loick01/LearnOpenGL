#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D objectTexture;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth){
    float z = depth * 2.0 - 1.0; // NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    FragColor = texture(objectTexture, TexCoords);

    // Visualizing the depth buffer
    // float depth = LinearizeDepth(gl_FragCoord.z) / 10.;
    // FragColor = vec4(vec3(depth), 1.0);
}