#version 130

in vec4 pos_in;
in vec3 tex_coord_in;
in vec3 norm_in;

out vec3 pos;
out vec3 norm;

uniform mat4 model_matrix;
uniform mat4 view_proj_matrix;
uniform mat3 normal_matrix;

void main() {
	norm = normal_matrix * norm_in;
	pos = vec3(model_matrix * pos_in);
	gl_Position = view_proj_matrix * vec4(pos, 1);
}
