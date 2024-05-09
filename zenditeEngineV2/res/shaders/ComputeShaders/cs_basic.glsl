#version 460 core

// Defines the size of each local work group (how many threads each work group creates)
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// 
layout(rgba32f, binding = 0) uniform image2D imgOutput;

void main()
{
	vec4 value = vec4(1.0, 0.0, 0.0, 1.0);
	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

	imageStore(imgOutput, texelCoord, value);
}