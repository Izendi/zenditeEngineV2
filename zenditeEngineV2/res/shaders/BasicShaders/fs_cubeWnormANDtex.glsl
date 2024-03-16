#version 420 core

in vec2 texCoord;
out vec4 FragColor;

//uniform vec4 ourColor;

uniform sampler2D colorTexture;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}



void main()
{
    vec4 texColor = texture(colorTexture, texCoord);

    if (texColor.a < 0.1)
    {
        discard; //stop processing this fragment and write nothing to the framebuffer.
    }

    FragColor = texColor;

    //float depth = LinearizeDepth(gl_FragCoord.z) / far;
    //FragColor = vec4(vec3(depth), 1.0);
    
}