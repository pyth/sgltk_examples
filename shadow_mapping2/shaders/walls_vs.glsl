#version 130

in vec4 pos_in;

out vec3 light;
out vec3 tc;

uniform mat4 model_matrix;
uniform mat4 view_proj_matrix;
uniform mat3 normal_matrix;
uniform mat4 light_matrix;

uniform vec3 light_pos;

void main() {
	vec4 pos = model_matrix * pos_in;
	tc = pos_in.xyz;

	light = light_pos - pos.xyz;

	gl_Position = view_proj_matrix * pos;
}
