#version 430

layout(std430, binding = 0) buffer data {float in_data[];};
layout(std430, binding = 1) buffer outpt {float out_data[];};
layout(local_size_x = 5, local_size_y = 1, local_size_z = 1) in;

void main() {
	out_data[gl_GlobalInvocationID.x] = sqrt(in_data[gl_GlobalInvocationID.x]);
}
