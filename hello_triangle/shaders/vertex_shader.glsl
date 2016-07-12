#version 130

in vec4 pos_in;
in vec4 color_in;

out vec4 color_fs;

uniform mat4 model_view_proj_matrix;

void main() {
	color_fs = color_in;
	gl_Position = model_view_proj_matrix * pos_in;
}
