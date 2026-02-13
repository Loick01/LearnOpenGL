#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light
{
    // Directional lights (only the direction)
    // vec3 direction; 

    // Point light with attenuation
    // vec3 position;
    // float constant;
    // float linear;
    // float quadractic;

    // Spotlight
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    // // Directional light
    // // Ambient
    // vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // // Diffuse
    // vec3 norm = normalize(Normal);
    // vec3 lightDir = normalize(-light.direction); // Must be inverted 
    // float diff = max(dot(norm, lightDir), 0.);
    // vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // // Specular
    // vec3 viewDir = normalize(viewPos-FragPos);
    // vec3 reflectDir = reflect(-lightDir, norm);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    // vec3 result = ambient + diffuse + specular;
    // FragColor = vec4(result, 1.0);

    // // Point light with attenuation
    // float distance = length(light.position - FragPos);
    // float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadractic * (distance*distance));
    
    // // Ambient
    // vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // // Diffuse
    // vec3 norm = normalize(Normal);
    // vec3 lightDir = normalize(light.position - FragPos);
    // float diff = max(dot(norm, lightDir), 0.);
    // vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // // Specular
    // vec3 viewDir = normalize(viewPos-FragPos);
    // vec3 reflectDir = reflect(-lightDir, norm);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    // vec3 result = ambient + diffuse + specular;
    // result *= attenuation;
    // FragColor = vec4(result, 1.0);

    // Spotlight
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);
    
    // Ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    if (theta > light.outerCutOff){ // Fragment inside the spotlight
        // Diffuse
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

        // Specular
        vec3 viewDir = normalize(viewPos-FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

        // Remove the 2 lines below to get hard edges
        diffuse *= intensity;
        specular *= intensity;
        // Ambient is unaffected to always keep some light 

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    } else { // Fragment outside the spotlight
        FragColor = vec4(ambient, 1.0);
    }
}