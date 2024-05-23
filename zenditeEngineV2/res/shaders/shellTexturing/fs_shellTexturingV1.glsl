#version 420 core

in vec2 texCoord;
in vec3 localPosition;

out vec4 FragColor;

//uniform vec4 ourColor;

uniform sampler2D colorTexture;

uniform sampler2D highTexture;
uniform sampler2D lowTexture;

uniform float baseNoGrassValue;
uniform float layerHeight;
//uniform float time;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}


void main()
{
    float gridSize = 400.0; //was 50

    // Calculate the grid cell coordinates
    vec2 gridPos = floor(texCoord * gridSize);

    float result = random(gridPos);

    vec4 texColor;

    if(layerHeight != 0)
    {
        if (localPosition.y > 14.0)
        {
            discard;
        }
    }

    if (result >= baseNoGrassValue * layerHeight) //baseNoGrassValue = 0.025 (is a good starting value)
    {
        texColor = texture(colorTexture, texCoord);
    }
    else
    {
        //texColor = vec4(0.0, 0.0, 0.0, 1.0);
        discard;
    }
    

    if (texColor.a < 0.1)
    {
        discard; //stop processing this fragment and write nothing to the framebuffer.
    }

    float intensity = layerHeight * 0.05;
    FragColor = texColor * (min(intensity, 1.0)) ;


    //FragColor = vec4(vec3(result), 1.0);
    
}