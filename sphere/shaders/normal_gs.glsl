#version 400

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec3 norm[];

uniform mat4 view_proj_matrix;

void main() {
	for(int i = 0; i < 3; i++) {
		gl_Position = view_proj_matrix * gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = view_proj_matrix * (gl_in[i].gl_Position + 0.3 * vec4(norm[i], 0));
		EmitVertex();
		EndPrimitive();
	}
}
