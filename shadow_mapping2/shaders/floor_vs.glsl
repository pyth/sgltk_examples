#version 130

in vec4 pos_in;
in vec3 norm_in;
in vec2 tc_in;

out vec4 pos_w;
out vec3 cam_vec;
out vec3 norm;
out vec2 tc;

uniform mat4 model_matrix;
uniform mat4 view_proj_matrix;
uniform mat3 normal_matrix;
uniform mat4 light_matrix;

uniform vec3 cam_pos;

void main() {
	pos_w = model_matrix * pos_in;
	tc = 15 * tc_in;

	cam_vec = cam_pos - pos_w.xyz;
	norm = normal_matrix * norm_in;

	gl_Position = view_proj_matrix * pos_w;
}
