#version 150

in vec3 pos_fs;

uniform vec3 light_pos;
uniform float far_plane;

void main() {
	gl_FragDepth = length(pos_fs - light_pos) / far_plane;
}
