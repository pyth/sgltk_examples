#version 130

in vec4 pos_in;
in vec3 tc_in;

out vec3 tc;

uniform vec2 resolution;
uniform mat4 model_matrix;

void main() {
	vec4 pos = model_matrix * pos_in;
	pos.x *= 512 / resolution.x;
	pos.y *= 512 / resolution.y;
	pos.x += 0.8;
	pos.y += 0.8;

	tc = tc_in;

	gl_Position = pos;
}
