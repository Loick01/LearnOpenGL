#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D objectTexture;
// layout (depth_greater) out float gl_FragDepth;

void main()
{
    FragColor = texture(objectTexture, TexCoords);

    // gl_FragCoord --> x and y are the screen coordinates of the current fragment, z component is equal to the depth value of this fragment 
    // vec3 result;
    // if (gl_FragCoord.x < 400){
    //     result = texture(objectTexture, TexCoords).rgb;
    // } else {
    //     result = texture(objectTexture, 1.0-TexCoords).rgb;
    //     result *= vec3(0.9846, 0.141, 0.092);
    // }
    // if (gl_FragCoord.z > 0.99){
    //     result *= vec3(0.31, 0.24, 0.98);
    // } 
    // FragColor = vec4(result, 1.0);

    // gl_FrontFacing --> bool value that is true if the current fragment belong to a front face, false otherwise (back face)
    // Of course, be sure the face culling is disabled before rendering with a shader that use this value

    // gl_FragDepth --> Depth value of the current fragment (gl_FragCoord.z is read only)
    // Writing in gl_FragDepth will disable early depth testing
}