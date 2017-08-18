#version 400

in vec3 pos_w;
in vec4 pos_ls;
in vec2 tc;
in vec3 norm;
in float height;

layout (location = 0) out vec3 position;
layout (location = 1) out vec4 position_ls;
layout (location = 2) out vec3 normals;
layout (location = 3) out float spec;

uniform float rock_mix_level;

void main() {
	position = pos_w;
	position_ls = pos_ls;
	normals = normalize(norm);
	if(height > rock_mix_level)
		spec = 0.7;
	else
		spec = 0;
}
