#version 420 core

in vec2 texCoord;
out vec4 FragColor;

uniform sampler3D colorTexture; //We need a 3D texture for the skydome.
uniform float iTime;

void main()
{
    vec3 texCoord3D;
    texCoord3D.x = texCoord.x;
    texCoord3D.y = texCoord.y;
    texCoord3D.z = sin(iTime * 0.1) * 0.5 + 0.5;

    vec4 texColor = texture(colorTexture, texCoord3D);

    if (texColor.a < 0.1)
    {
        discard; //stop processing this fragment and write nothing to the framebuffer.
    }

    FragColor = texColor;

}