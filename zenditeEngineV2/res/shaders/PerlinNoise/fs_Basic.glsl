#version 420 core

in vec2 texCoord;
in vec4 fragPos;

out vec4 FragColor;

uniform float deltaTime;

void main()
{
    // Perform perspective division to get normalized device coordinates (if needed)
    //vec3 ndcPos = fragPos.xyz / fragPos.w; // Uncomment if fragPos is in clip space

    // Map NDC coordinates [-1, 1] to [0, 1]
    vec3 normalizedPos = (fragPos.xyz + vec3(1.0)) * 0.5;

    // Create an oscillating color effect using a sine wave
    float oscillation = sin(deltaTime) * 0.5 + 0.5; // Oscillates between 0 and 1

    // Output normalized coordinates directly for debugging
    FragColor = vec4(normalizedPos * oscillation, 1.0); // Color based on normalized position
}