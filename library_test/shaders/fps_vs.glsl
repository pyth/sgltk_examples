#version 130

struct Matrix {
	mat4 proj_matrix;
	mat4 model_view;
};

in vec4 pos_in;
in vec3 tex_coord_in0;

out vec3 tc;

uniform mat4 model_view_matrix;
uniform mat4 proj_matrix;
uniform vec2 Resolution;
uniform sampler2D Texture;

void main() {
	vec4 pos = model_view_matrix * pos_in;

	pos.x += 0.95 * Resolution.x;
	pos.y += 0.97 * Resolution.y;

	tc = tex_coord_in0;

	gl_Position = proj_matrix * pos;
}
