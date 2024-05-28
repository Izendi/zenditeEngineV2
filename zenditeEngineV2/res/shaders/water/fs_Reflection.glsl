#version 420 core

in vec2 texCoord;
out vec4 FragColor;

//uniform vec4 ourColor;

uniform sampler2D fboReflectionTexture;


void main()
{
    vec4 texColor = texture(fboReflectionTexture, texCoord);

    if (texColor.a < 0.1)
    {
        discard; //stop processing this fragment and write nothing to the framebuffer.
    }

    FragColor = texColor;

    //float depth = LinearizeDepth(gl_FragCoord.z) / far;
    //FragColor = vec4(vec3(depth), 1.0);
    
}