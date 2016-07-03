#version 130

in vec4 pos_in;
in vec3 norm_in;
in ivec4 bone_ids_in;
in vec4 bone_weights_in;

out vec4 pos_eye;
out vec4 light_vec;
out vec4 norm;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 view_proj_matrix;
uniform mat3 normal_matrix;
uniform mat4[40] bone_array;
uniform vec3 light_pos;

void main() {
	mat4 bone_transformation =	bone_array[bone_ids_in[0]] * bone_weights_in[0];
	bone_transformation +=		bone_array[bone_ids_in[1]] * bone_weights_in[1];
	bone_transformation +=		bone_array[bone_ids_in[2]] * bone_weights_in[2];
	bone_transformation +=		bone_array[bone_ids_in[3]] * bone_weights_in[3];

	vec4 pos = model_matrix * bone_transformation * pos_in;

	pos_eye = view_matrix * pos;
	light_vec = view_matrix * (vec4(light_pos, 1.0) - pos);
	norm = view_matrix * vec4(normal_matrix * (bone_transformation * vec4(norm_in, 0)).xyz, 0);

	gl_Position = view_proj_matrix * pos;
}
