#version 330

in vec4 pos_in;
in vec3 tex_coord_in;

out vec3 tc;

uniform mat4 model_view_proj_matrix;

void main() {
	tc = pos_in.xyz;
	gl_Position = model_view_proj_matrix * pos_in;
}
