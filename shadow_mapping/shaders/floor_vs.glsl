#version 130

in vec4 pos_in;
in vec3 norm_in;

out vec3 cam_vec;
out vec3 norm;
out vec3 pos_ls;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 view_proj_matrix;
uniform mat3 normal_matrix;
uniform mat4 light_matrix;

uniform vec3 cam_pos;

void main() {
	vec4 pos = model_matrix * pos_in;

	cam_vec = cam_pos - pos.xyz;
	norm = normal_matrix * norm_in;
	vec4 p_ls = light_matrix * pos;
	pos_ls = p_ls.xyz / p_ls.w;

	gl_Position = view_proj_matrix * pos;
}
