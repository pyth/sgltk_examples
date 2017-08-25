#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 9) out;

in vec3 pos_w[3];

uniform mat4 light_matrix[3];

void main() {
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			gl_Layer = i;
			gl_Position = light_matrix[i] * vec4(pos_w[j], 1);
			EmitVertex();
		}
		EndPrimitive();
	}
}
