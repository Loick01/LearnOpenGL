#version 460 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform sampler2D floorTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float shininess;
uniform bool use_BlinnPhong;

void main()
{           
    vec3 color = texture(floorTexture, TexCoords).rgb;

    vec3 ambient = 0.05 * color;
    
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    if (use_BlinnPhong){
        vec3 halfwayDir = normalize(lightDir + viewDir); 
        spec = pow(max(dot(normal, halfwayDir), 0.0), shininess*3.0);
    }else{
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    }
    vec3 specular = vec3(0.3) * spec;
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}