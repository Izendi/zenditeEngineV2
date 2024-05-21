#version 420 core

out vec4 FragColor;

in vec2 TexCoords;
in vec2 OffsetTexCoords[4];

uniform sampler2D noiseTexture;

void main()
{
    float noiseR = texture(noiseTexture, OffsetTexCoords[0]).r;
    float noiseG = texture(noiseTexture, OffsetTexCoords[1]).g;
    float noiseB = texture(noiseTexture, OffsetTexCoords[2]).b;
    float noiseA = texture(noiseTexture, OffsetTexCoords[3]).a;

    // Combine the octaves
    float fbm = (noiseR + noiseG + noiseB + noiseA) / 4.0;

    FragColor = vec4(vec3(fbm), 1.0); // Output final cloud texture
}