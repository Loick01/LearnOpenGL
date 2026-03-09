#version 460 core

out float FragColor;

in vec2 TexCoords;

struct Textures // Textures from the G-Buffer
{
    sampler2D position;
    sampler2D normal;
    sampler2D noise;
};

uniform Textures gTextures;
uniform mat4 projection;
uniform vec3 samples[64];
uniform float screenWidth;
uniform float screenHeight;

int kernelSize = 64;
float radius = 1.;
float bias = 0.025;

// The noise texture (4x4) will be tiled over the screen 
const vec2 noiseScale = vec2(screenWidth/4.0, screenHeight/4.0); 

void main()
{
    vec3 fragPos = texture(gTextures.position, TexCoords).xyz;
    vec3 normal = normalize(texture(gTextures.normal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(gTextures.noise, TexCoords * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 samplePos = TBN * samples[i]; // From tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // From view to clip-space
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; // [0., 1.]
        
        float sampleDepth = texture(gTextures.position, offset.xy).z; // Depth value of kernel sample
        
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = occlusion; // Use pow(occlusion, power) to increase the final occlusion
}