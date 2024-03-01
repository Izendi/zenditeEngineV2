#version 420 core
layout(location = 0) in vec3 aPos;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

uniform vec3 lightRGB;

out vec3 lightColor;

void main()
{
    mat4 MVP = projMat * viewMat * modelMat;

    lightColor = lightRGB * 3.0;

    gl_Position = MVP * vec4(aPos, 1.0);

}