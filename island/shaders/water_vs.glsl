#version 330

in vec4 pos_in;
in vec2 tc_in;

out vec3 pos_w;
out vec4 pos_ls;
out vec4 pos;
out vec2 tc;

uniform float time;
uniform uint terrain_side;
uniform mat4 light_matrix[3];
uniform mat4 model_matrix;
uniform mat4 view_proj_matrix;

void main() {
	vec2 flow_direction = vec2(-1, 3);

	pos = model_matrix * pos_in;
	pos_w = pos.xyz;
	pos_ls = 0.5f * light_matrix[0] * pos + vec4(vec3(0.5), 0);
	pos = view_proj_matrix * pos;

	tc = (tc_in + normalize(flow_direction) * time / 5000) * terrain_side;
	gl_Position = pos;
}
