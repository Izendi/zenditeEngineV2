#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 ClippingPlane;


void main()
{

    vec4 worldPos = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_ClipDistance[0] = dot(worldPos, ClippingPlane);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    texCoord = aTexCoord;
}