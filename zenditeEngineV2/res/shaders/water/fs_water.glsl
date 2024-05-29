#version 420 core

//This shader uses OpenGL water Tutorial by ThinMatrix on youtube as a basis:
//Link: https://www.youtube.com/watch?v=6B7IF6GOu7s

in vec2 texCoord;
in vec3 FragPos;
in vec4 clipSpace;
out vec4 FragColor;

//uniform vec4 ourColor;

struct Material
{
    //sampler2D colorTexture;

    //vec3 ambient;
    //vec3 diffuse;
    //vec3 specular;
    float shininess;
};

struct Light
{
    vec3 direction;

    //vec3 ambient;
    //vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform sampler2D waterReflectionTexture;
uniform sampler2D waterRefractionTexture;

uniform sampler2D colorTexture;
uniform sampler2D normalMap;

uniform float rippleOffset;

uniform vec3 currentSkyColor;

const float strength = 0.015;

void main()
{
    vec2 ndc = (clipSpace.xy / clipSpace.w)/2.0 + 0.5;
    
    vec2 reflectionTexCoord = vec2(ndc.x, -ndc.y);
    vec2 refractionTexCoord = vec2(ndc.x, ndc.y);


    //vec2 flippedTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);

    vec2 distortion_1 = (texture(colorTexture, vec2(texCoord.x, texCoord.y + rippleOffset)).rg * 2.0 - 1.0) * strength;
    vec2 distortion_2 = (texture(colorTexture, vec2(-texCoord.x + rippleOffset, texCoord.y)).rg * 2.0 - 1.0) * strength;
    
    vec2 combinedDistortion = distortion_1 + distortion_2;

    reflectionTexCoord += combinedDistortion;
    reflectionTexCoord.x = clamp(reflectionTexCoord.x, 0.01, 0.99);
    reflectionTexCoord.y = clamp(reflectionTexCoord.y, -0.99, -0.01);
    

    refractionTexCoord += combinedDistortion;
    //refractionTexCoord.x = clamp(refractionTexCoord.x, 0.01, 0.99);
    //refractionTexCoord.y = clamp(refractionTexCoord.y, -0.99, -0.01);

    vec4 reflectionColor = texture(waterReflectionTexture, reflectionTexCoord);
    vec4 refractionColor = texture(waterRefractionTexture, refractionTexCoord);

    vec4 normalMapColor = texture(normalMap, combinedDistortion);
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0);
    
    normal = normalize(normal);

    vec3 lightDir = light.direction; 

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec;// * material.specular;

    vec3 lightResult = specular;

    //FragColor = texColor;
    vec4 finalColor = mix(reflectionColor, refractionColor, 0.5);
    
    finalColor = mix(finalColor, vec4(0.0, 0.2, 0.4, 1.0), 0.2);
    finalColor = mix(finalColor, vec4(currentSkyColor.r, currentSkyColor.g, currentSkyColor.b, 1.0), 0.2);

    FragColor = finalColor + vec4(lightResult, 0.0); //Add a unform for the iterpolation factor that can be controlled by the menu
    //FragColor = normalMapColor;
}