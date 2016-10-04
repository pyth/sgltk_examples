#version 130

in vec4 color_fs;

out vec4 color;

void main() {
	color = vec4(1) - color_fs;
}
