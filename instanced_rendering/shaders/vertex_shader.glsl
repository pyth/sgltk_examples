#version 330

layout (location = 0) in vec4 pos_in;
layout (location = 1) in vec4 color_in;
layout (location = 2) in mat4 mod_mat;

out vec4 color_fs;

uniform mat4 view_proj_matrix;

void main() {
	color_fs = color_in;
	gl_Position = view_proj_matrix * mod_mat * pos_in;
}
