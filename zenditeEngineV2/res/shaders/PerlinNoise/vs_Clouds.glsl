#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec2 OffsetTexCoords[4];

uniform vec2 speed[4];
uniform float time;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoords = aTexCoords;
    for (int i = 0; i < 4; ++i) {
        OffsetTexCoords[i] = aTexCoords + speed[i] * time;
    }
}