#version 150

in vec4 pos_in;

uniform mat4 model_matrix;

void main() {
	gl_Position = model_matrix * pos_in;
}
