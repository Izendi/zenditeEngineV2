#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoord;

out vec2 texCoord;

out vec4 clipSpace;

out vec3 FragPos;

out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightSpaceMatrix;

const float tiling = 6.0;

void main()
{
    
    vec4 worldPos = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);

    clipSpace = projection * view * worldPos;;

    gl_Position = clipSpace;
    //texCoord = aTexCoord;
    texCoord = vec2(aPos.x / 2.0 + 0.5, aPos.z/2.0 + 0.5) * tiling;

    FragPos = vec3(model * vec4(aPos, 1.0));

    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}