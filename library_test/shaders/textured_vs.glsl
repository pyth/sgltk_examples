#version 130

in vec4 pos_in;
in vec4 tang_in;
in vec3 norm_in;
in vec3 tex_coord_in0;

in ivec4 bone_ids_in;
in vec4 bone_weights_in;

out vec3 cam_vec;
out vec3 light_vec;
out vec3 tc;

uniform vec3 light_pos;
uniform vec3 cam_pos;
uniform mat4 model_matrix;
uniform mat4 view_proj_matrix;
uniform mat3 normal_matrix;
uniform mat4[100] bone_array;

void main() {
	mat4 bone_transformation =	bone_array[bone_ids_in[0]] * bone_weights_in[0];
	bone_transformation +=		bone_array[bone_ids_in[1]] * bone_weights_in[1];
	bone_transformation +=		bone_array[bone_ids_in[2]] * bone_weights_in[2];
	bone_transformation +=		bone_array[bone_ids_in[3]] * bone_weights_in[3];

	vec3 norm = normalize(normal_matrix * (bone_transformation * vec4(norm_in, 0)).xyz);
	vec3 tang = normalize(normal_matrix * (bone_transformation * tang_in).xyz);
	vec3 bitang = normalize(cross(norm, tang));
	mat3 tangent_matrix = transpose(mat3(tang, bitang, norm));

	vec4 pos = model_matrix * bone_transformation * pos_in;

	cam_vec = tangent_matrix * (cam_pos - pos.xyz);
	light_vec = tangent_matrix * (light_pos - pos.xyz);

	tc = tex_coord_in0;

	gl_Position = view_proj_matrix * pos;
}
