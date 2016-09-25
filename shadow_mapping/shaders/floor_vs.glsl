#version 330

layout(location = 0) in vec4 pos_in;
layout(location = 1) in vec3 norm_in;

out vec3 cam_vec;
out vec3 light_vec;
out vec3 norm;
out vec3 pos_ls;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 view_proj_matrix;
uniform mat3 normal_matrix;
uniform mat4 light_matrix;

uniform vec3 camera_pos;
uniform vec3 light_pos;

void main() {
	vec4 pos = model_matrix * pos_in;

	cam_vec = camera_pos - pos.xyz;
	light_vec = light_pos - pos.xyz;
	norm = normal_matrix * norm_in;
	vec4 pos_light_space = light_matrix * pos;
	pos_ls = (pos_light_space.xyz / pos_light_space.w + 1) / 2;

	gl_Position = view_proj_matrix * pos;
}
