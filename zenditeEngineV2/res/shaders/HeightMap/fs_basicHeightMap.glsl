#version 420 core

//Instead of setting the material color we need to sample from a texture.
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess; //AKA: specularStrength
};

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 fragPos;
in vec3 Normal;
in vec2 texCoords;

out vec4 FragColor;

uniform Material material;
uniform Light light;

uniform vec3 lightColor;

uniform vec3 lightWorldPos;
uniform vec3 cameraWorldPos;

void main()
{
    vec3 lightDir = normalize(lightWorldPos - fragPos);
    vec3 norm = normalize(Normal);

    float distance = length(lightWorldPos - fragPos);
    //float lightAttenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //ambient light:
    vec3 ambient = light.ambient * texture(material.diffuse, texCoords).rgb;
    ambient = ambient; //*lightAttenuation;

    //diffuse Light:
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * texture(material.diffuse, texCoords).rgb;
    diffuse = diffuse; //*lightAttenuation;

    //Specular Light:
    vec3 reflection = normalize(2 * (dot(norm, lightDir)) * norm - lightDir);
    vec3 viewDir = normalize(cameraWorldPos - fragPos);
    float spec = pow(max(dot(reflection, viewDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, texCoords).rgb;
    specular = specular; //*lightAttenuation;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}
