#version 420 core

out vec4 FragColor;

in vec2 TexCoords;
in vec2 OffsetTexCoords[4];

uniform sampler2D noiseTexture;
uniform float discardThreshold;

float mapRange(float v, float x) {
    return (v - x) / (1.0 - x);
}

void main()
{
    float noiseR = texture(noiseTexture, OffsetTexCoords[0]).r;
    float noiseG = texture(noiseTexture, OffsetTexCoords[1]).g;
    float noiseB = texture(noiseTexture, OffsetTexCoords[2]).b;
    float noiseA = texture(noiseTexture, OffsetTexCoords[3]).a;

    // Combine the octaves
    float fbm = (noiseR + noiseG + noiseB + noiseA) / 4.0;

    vec4 finalColor = vec4(fbm, fbm, fbm, 0.5);
    vec4 skyColor = vec4(0.2f, 0.3f, 0.3f, 0.5);
    
    if (fbm < discardThreshold)
    {
        //FragColor = skyColor;
        discard;
    }
    else
    {
        //FragColor = vec4(vec3(fbm), 1.0); // Output final cloud texture

        fbm = mapRange(fbm, discardThreshold);

        FragColor = mix(skyColor, vec4(vec3(fbm + 0.5), 0.5), 0.7 );
    }

}