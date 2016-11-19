#version 150

in mat4 model_matrix;

in vec4 pos_in;

void main() {
	gl_Position = model_matrix * pos_in;
}
