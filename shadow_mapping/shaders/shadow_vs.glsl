#version 130

in vec4 pos_in;

uniform mat4 model_view_proj_matrix;

void main() {
	gl_Position = model_view_proj_matrix * pos_in;
}
