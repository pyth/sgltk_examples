#version 150

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 light_matrix[6];

out vec3 pos_fs;

void main() {
	for(int side = 0; side < 6; side++) {
		for(int i = 0; i < 3; i++) {
			gl_Layer = side;
			pos_fs = gl_in[i].gl_Position.xyz;
			gl_Position = light_matrix[side] * gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}
