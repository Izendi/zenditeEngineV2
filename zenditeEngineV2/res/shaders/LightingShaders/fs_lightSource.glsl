#version 420 core
in vec3 lightColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(lightColor.x, lightColor.y, lightColor.z, 1.0); // set all 4 vector values to 1.0
}