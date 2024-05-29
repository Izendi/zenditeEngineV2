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
    /*
    vec4 texColor = texture(colorTexture, texCoord);
    */

    float depthValue = texture(colorTexture, texCoord).r;
    
    depthValue += -0.8;
    
    depthValue = (depthValue - 0.5) / (1.0 + 0.9) + 0.5;

    
    /*
    if (texColor.a < 0.1)
    {
        discard; //stop processing this fragment and write nothing to the framebuffer.
    }
    */

    /*
    FragColor = texColor;
    */

    FragColor = vec4(vec3(depthValue), 1.0);
    
}