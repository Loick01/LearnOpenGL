#version 460 core

in VERTEX_OUT {
    vec2 TexCoords;
} fragment_in;

out vec4 FragColor;

uniform sampler2D objectTexture;
// layout (depth_greater) out float gl_FragDepth;

void main()
{
    // FragColor = texture(objectTexture, fragment_in.TexCoords);

    // gl_FragCoord --> x and y are the screen coordinates of the current fragment, z component is equal to the depth value of this fragment 
    vec3 result;
    if (gl_FragCoord.x < 400){
        result = texture(objectTexture, fragment_in.TexCoords).rgb;
    } else {
        result = texture(objectTexture, 1.0-fragment_in.TexCoords).rgb;
        result *= vec3(0.95, 0.05, 0.1);
    }
    if (gl_FragCoord.z > 0.99){
        result *= vec3(0.1, 0.95, 0.05);
    } 
    FragColor = vec4(result, 1.0);

    // gl_FrontFacing --> bool value that is true if the current fragment belong to a front face, false otherwise (back face)
    // Of course, be sure the face culling is disabled before rendering with a shader that use this value

    // gl_FragDepth --> Depth value of the current fragment (gl_FragCoord.z is read only)
    // Writing in gl_FragDepth will disable early depth testing
}