#version 420 core

in vec2 texCoord;
in vec4 clipSpace;
out vec4 FragColor;

//uniform vec4 ourColor;

uniform sampler2D waterReflectionTexture;
uniform sampler2D waterRefractionTexture;

uniform sampler2D colorTexture;


void main()
{
    vec2 ndc = (clipSpace.xy / clipSpace.w)/2.0 + 0.5;
    
    vec2 reflectionTexCoord = vec2(ndc.x, -ndc.y);
    vec2 refractionTexCoord = vec2(ndc.x, ndc.y);


    //vec2 flippedTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);

    vec4 texColor = texture(colorTexture, texCoord);

    vec4 reflectionColor = texture(waterReflectionTexture, reflectionTexCoord);
    vec4 refractionColor = texture(waterRefractionTexture, refractionTexCoord);


    FragColor = texColor;

    //FragColor = mix(reflectionColor, refractionColor, 0.0); //Add a unform for the iterpolation factor that can be controlled by the menu

}