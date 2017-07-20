#version 400

in vec4 pos_in;
in vec2 tc_in;

out vec2 tc;

void main() {
	tc = tc_in;
	gl_Position = pos_in;
}
