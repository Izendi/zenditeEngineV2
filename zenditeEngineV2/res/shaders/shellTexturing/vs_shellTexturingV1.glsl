#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 localPosition;

out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float layerHeight;
uniform float time;

uniform vec4 ClippingPlane;

uniform mat4 lightSpaceMatrix;

void main()
{
    //float sinPhaseShift = float(layerHeight) * 0.5;

    vec4 worldPos = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_ClipDistance[0] = dot(worldPos, ClippingPlane);
    
    float sway = (sin(time) + 0.9)/2 ; //+ sinPhaseShift);

    vec3 finalPos = aPos;
    
    
    finalPos.x += sway * (0.06 * layerHeight);

    gl_Position = projection * view * model * vec4(finalPos, 1.0);
    texCoord = aTexCoord;
    localPosition = aPos;
    Normal = aNorm;
    FragPos = vec3(model * vec4(aPos, 1.0));

    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}