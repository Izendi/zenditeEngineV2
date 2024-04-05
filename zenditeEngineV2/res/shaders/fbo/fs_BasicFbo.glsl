#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    //Default output:
    
    FragColor = texture(screenTexture, TexCoords);
    

    //Color Inversion:
    /*
    FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
    */

    //Grayscale (all colour brightness assumed even):
    /*
    FragColor = texture(screenTexture, TexCoords);
    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;

    FragColor = vec4(average, average, average, 1.0);
    */

    //Greyscale 2 (where we try to account for diffwerent colors like green apearing brighter than red and blue to the human eye)
    /*
    FragColor = texture(screenTexture, TexCoords);
    float average = (0.299 * FragColor.r + 0.587 * FragColor.g + 0.114 * FragColor.b) / 3.0;
    average = average * 2; //doubled the average as the scene can be quite dim by default.
    FragColor = vec4(average, average, average, 1.0);
    */


}