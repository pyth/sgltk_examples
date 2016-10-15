#version 130

in vec4 pos_in;
in vec3 tex_coord_in;
in vec3 norm_in;

out vec3 cam_vec;
out vec3 norm;

uniform vec3 cam_pos;
uniform mat4 model_matrix;
uniform mat4 view_proj_matrix;
uniform mat3 normal_matrix;

void main() {
	norm = normal_matrix * norm_in;
	vec4 pos = model_matrix * pos_in;
	cam_vec = normalize(cam_pos - pos.xyz);
	gl_Position = view_proj_matrix * pos;
}
