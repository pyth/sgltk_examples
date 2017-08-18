#version 330

in vec3 cam_vec;
in vec3 pos_w;
in vec4 pos_ls;
in vec4 pos;
in vec2 tc;

layout (location = 0) out vec3 position;
layout (location = 1) out vec4 position_ls;
layout (location = 2) out vec3 normals;
layout (location = 3) out float spec;

void main() {
	position = pos_w;
	position_ls = pos_ls;
	normals = vec3(0, 1, 0);
	spec = 0.7;
}
