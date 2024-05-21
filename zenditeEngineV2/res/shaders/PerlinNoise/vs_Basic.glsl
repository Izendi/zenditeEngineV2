#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 texCoord;
out vec4 fragPos;

void main()
{
    fragPos = vec4(aPos, 1.0);
    gl_Position = fragPos;
    texCoord = aTexCoord;
}