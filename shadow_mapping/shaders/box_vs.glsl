#version 130

in vec4 pos_in;
in vec3 norm_in;
in vec3 tex_coord_in0;

out vec3 cam_vec;
out vec3 light_vec;
out vec3 norm;
out vec2 tc;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 view_proj_matrix;
uniform mat3 normal_matrix;

uniform vec3 camera_pos;
uniform vec3 light_pos;

void main() {
	vec4 pos = model_matrix * pos_in;

	cam_vec = -(view_matrix * pos).xyz;
	light_vec = (view_matrix * (vec4(light_pos, 1) - pos)).xyz;
	norm = (view_matrix * vec4(normal_matrix * norm_in, 0)).xyz;
	tc = tex_coord_in0.xy;

	gl_Position = view_proj_matrix * pos;
}
