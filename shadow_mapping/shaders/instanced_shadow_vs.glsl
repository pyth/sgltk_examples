#version 130

in mat4 model_matrix;

in vec4 pos_in;

uniform mat4 view_proj_matrix;

void main() {
	gl_Position = view_proj_matrix * model_matrix * pos_in;
}
