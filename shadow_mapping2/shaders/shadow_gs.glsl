#version 150

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 light_matrix[6];

out vec4 pos_fs;

void main() {
	for(int side = 0; side < 6; side++) {
		gl_Layer = side;
		for(int i = 0; i < 3; i++) {
			pos_fs = gl_in[i].gl_Position;
			gl_Position = light_matrix[side] * pos_fs;
			EmitVertex();
		}
		EndPrimitive();
	}
}
