#version 420 core

//This shader uses OpenGL water Tutorial by ThinMatrix on youtube as a basis:
//Link: https://www.youtube.com/watch?v=6B7IF6GOu7s

in vec2 texCoord;
in vec3 FragPos;
in vec4 clipSpace;

in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform sampler2D shadowMap;

uniform float waterShadowIntensity;

//uniform vec4 ourColor;

//This functions code was taken from: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
float ShadowCalculation(vec4 FragPosLightSpace)
{
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow * waterShadowIntensity;
}


struct Material
{
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

uniform sampler2D refractionDepthTexture;

uniform sampler2D colorTexture;
uniform sampler2D normalMap;

uniform float rippleOffset;

uniform vec3 currentSkyColor;

const float strength = 0.015;

void main()
{
    float shadow = ShadowCalculation(FragPosLightSpace);

    vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;

    vec2 reflectionTexCoord = vec2(ndc.x, -ndc.y);
    vec2 refractionTexCoord = vec2(ndc.x, ndc.y);

    float near = 0.1;
    float far = 100.0;
    float depth = texture(refractionDepthTexture, refractionTexCoord).r;
    float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

    depth = gl_FragCoord.z;
    float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

    float waterDepth = floorDistance - waterDistance;

    //vec2 flippedTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);

    vec2 distortion_1 = (texture(colorTexture, vec2(texCoord.x, texCoord.y + rippleOffset)).rg * 2.0 - 1.0) * strength;
    vec2 distortion_2 = (texture(colorTexture, vec2(-texCoord.x + rippleOffset, texCoord.y)).rg * 2.0 - 1.0) * strength;

    vec2 combinedDistortion = distortion_1 + distortion_2 * clamp(waterDepth / 0.3, 0.0, 1.0);

    reflectionTexCoord += combinedDistortion;
    reflectionTexCoord.x = clamp(reflectionTexCoord.x, 0.0001, 0.9999);
    reflectionTexCoord.y = clamp(reflectionTexCoord.y, -0.9999, -0.0001);


    refractionTexCoord += combinedDistortion;
    refractionTexCoord = clamp(refractionTexCoord, 0.001, 0.999);
    

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

    vec3 lightResult = specular * (1.0 - shadow) * clamp(waterDepth / 0.09, 0.0, 1.0);

    //FragColor = texColor;
    vec4 finalColor = mix(reflectionColor, refractionColor, 0.5);

    finalColor = mix(finalColor, vec4(0.0, 0.2, 0.4, 1.0), 0.2);
    //finalColor = mix(finalColor, vec4(currentSkyColor.r/2.0, currentSkyColor.g/2.0, currentSkyColor.b/2.0, 1.0), 0.01);
    //finalColor = mix(finalColor, vec4(0.851, 0.835, 0.059, 1.0), 0.5);

    FragColor = finalColor + vec4(lightResult, 0.5); //Add a unform for the iterpolation factor that can be controlled by the menu
    FragColor.a = clamp(waterDepth/0.05, 0.5, 1.0);
    
    //FragColor = normalMapColor;
    //FragColor = vec4(waterDepth/1.0);
}