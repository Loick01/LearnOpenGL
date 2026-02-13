#version 460 core

in vec3 Normal;
in vec3 Position;

out vec4 FragColor;

uniform samplerCube skyboxTexture;
uniform vec3 cameraPos;

void main()
{
    // Reflection cubes
    vec3 I = normalize(Position-cameraPos); // Camera position to fragment world position
    vec3 R = reflect(I, normalize(Normal));
    vec3 result = texture(skyboxTexture, R).rgb * vec3(1.0, 0.1, 0.1);
    FragColor = vec4(result, 1.0);

    // Refraction cubes
    // float ratio = 1.0 / 1.52;
    // vec3 I = normalize(Position-cameraPos); // Camera position to fragment world position
    // vec3 R = refract(I, normalize(Normal), ratio);
    // FragColor = vec4(texture(skyboxTexture, R).rgb, 1.0);
}