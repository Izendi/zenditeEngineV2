#version 420 core


in vec2 texCoord;
in vec4 fragPos;

out vec4 FragColor;

//uniform vec4 ourColor;

//The below is a method of getting the texture from textue unit 0 without manually having to send it in yourself.
//layout(binding = 0) uniform sampler2D colortexture;

void main()
{
        //vec2 fragPos = gl_FragCoord.xy;

        FragColor = vec4(fragPos.x * 255, fragPos.y * 255, 0, 1);

}