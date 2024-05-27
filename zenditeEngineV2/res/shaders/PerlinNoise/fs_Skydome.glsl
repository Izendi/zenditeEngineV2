#version 420 core

in vec3 FragPos;
in vec3 Normal;
in vec2 texCoord;

out vec4 FragColor;


//uniform vec4 ourColor;

struct Light {
    //vec3 position;

    vec3 cloudAmbient;
    vec3 cloudDiffuse;
    //vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform vec3 lightPosition;

uniform Light light;

uniform float angleRadians;

uniform sampler2D colorTexture;

void main()
{
    vec4 texColor = texture(colorTexture, texCoord);
    vec3 texColor_3 = vec3(texColor.x, texColor.y, texColor.z);

    float CA = max(sin(angleRadians), 0.0);
    float CD = 1.2;

    // ambient
    vec3 ambient = CA * texColor_3;

    // diffuse 
    
    vec3 norm = normalize(FragPos - vec3(0, 0, 0)); //normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = CD * diff * texColor_3;
    
    /*
    vec3 lightDir = normalize(vec3(1.0f, 5.0f, 22.0f) - FragPos);
    vec3 norm = normalize(FragPos - vec3(0, 0, 0)); //normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = CD * diff * texColor_3;
    */

    // attenuation
    float distance = length(lightPosition - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //ambient *= attenuation;
    //diffuse *= attenuation;

    vec3 result = ambient + diffuse;

    

    if (texColor.a < 0.1)
    {
        discard; //stop processing this fragment and write nothing to the framebuffer.
    }
    
    

    FragColor = vec4(result, texColor.a);

    //FragColor = vec4(lightDir * 0.5 + 0.5, 1.0); // Map from [-1, 1] to [0, 1]
    return;

    //FragColor = texture(colorTexture, texCoord);// *vec4(result.x, result.y, result.z, 1.0);

    //float depth = LinearizeDepth(gl_FragCoord.z) / far;
    //FragColor = vec4(vec3(depth), 1.0);

}