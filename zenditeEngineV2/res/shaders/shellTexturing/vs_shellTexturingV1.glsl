#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 localPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float layerHeight;
uniform float time;

void main()
{
    //float sinPhaseShift = float(layerHeight) * 0.5;
    
    float sway = (sin(time) + 0.9)/2 ; //+ sinPhaseShift);

    vec3 finalPos = aPos;
    
    
    finalPos.x += sway * (0.06 * layerHeight);

    gl_Position = projection * view * model * vec4(finalPos, 1.0);
    texCoord = aTexCoord;
    localPosition = aPos;
}