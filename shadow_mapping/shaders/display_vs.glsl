#version 130

in vec4 pos_in;
in vec3 tc_in;

out vec3 tc;

uniform mat4 model_matrix;

void main() {
	vec4 pos = vec4((model_matrix * pos_in).xy * 0.125, 0, 1);
	pos.x += 0.9;
	pos.y += 0.9;

	tc = tc_in;

	gl_Position = pos;
}
