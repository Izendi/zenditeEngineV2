#version 420 core

//This shader uses OpenGL water Tutorial by ThinMatrix on youtube as a basis:
//Link: https://www.youtube.com/watch?v=6B7IF6GOu7s

in vec2 texCoord;
in vec4 clipSpace;
out vec4 FragColor;

//uniform vec4 ourColor;

uniform sampler2D waterReflectionTexture;
uniform sampler2D waterRefractionTexture;

uniform sampler2D colorTexture;

uniform float rippleOffset;

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
    reflectionTexCoord.y = clamp(reflectionTexCoord.y, -0.999, -0.001);
    reflectionTexCoord.x = clamp(reflectionTexCoord.x, 0.001, 0.999);

    refractionTexCoord += distortion_1;
    

    vec4 reflectionColor = texture(waterReflectionTexture, reflectionTexCoord);
    vec4 refractionColor = texture(waterRefractionTexture, refractionTexCoord);

    
    //FragColor = texColor;
    vec4 finalColor = mix(reflectionColor, refractionColor, 0.5);
    finalColor = mix(finalColor, vec4(0.0, 0.2, 0.4, 1.0), 0.2);

    FragColor = finalColor; //Add a unform for the iterpolation factor that can be controlled by the menu

}