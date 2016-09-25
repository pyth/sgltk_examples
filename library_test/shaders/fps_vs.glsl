#version 130

in vec4 pos_in;
in vec3 tex_coord_in0;

out vec3 tc;

uniform vec2 Resolution;

void main() {
	vec4 pos = vec4(pos_in.xy * 0.125, 0, 1);
	pos.x += 0.9;
	pos.y += 0.9;

	tc = tex_coord_in0;

	gl_Position = pos;
}
