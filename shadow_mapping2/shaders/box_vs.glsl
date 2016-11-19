#version 130

in mat4 model_matrix;
in mat3 normal_matrix;

in vec4 pos_in;
in vec3 norm_in;
in vec3 tex_coord_in0;

out vec3 cam_vec;
out vec3 norm;
out vec2 tc;
out vec4 pos_ls;
out vec3 light;

uniform vec3 cam_pos;
uniform vec3 light_pos;
uniform mat4 light_matrix;
uniform mat4 view_proj_matrix;

void main() {
	vec4 pos = model_matrix * pos_in;

	cam_vec = cam_pos - pos.xyz;
	norm = normal_matrix * norm_in;
	tc = tex_coord_in0.xy;
	pos_ls = light_matrix * pos;
	light = light_pos - pos.xyz;

	gl_Position = view_proj_matrix * pos;
}
