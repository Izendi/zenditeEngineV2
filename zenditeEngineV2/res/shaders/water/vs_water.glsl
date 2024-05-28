#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoord;

out vec2 texCoord;

out vec4 clipSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    

    vec4 worldPos = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);

    clipSpace = projection * view * worldPos;;

    gl_Position = clipSpace;
    //texCoord = aTexCoord;
    texCoord = aTexCoord;


}