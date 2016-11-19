#version 150

in vec4 pos_fs;

uniform vec3 light_pos;
uniform float far_plane;

void main() {
	gl_FragDepth = length(pos_fs.xyz - light_pos) / far_plane;
}
