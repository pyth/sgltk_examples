#version 400

layout (triangles, equal_spacing, ccw) in;

out vec3 light;
out vec3 norm;

uniform vec3 light_pos;
uniform mat4 model_matrix;
uniform mat4 model_view_proj_matrix;
uniform mat3 normal_matrix;

void main() {
	vec4 pos_tmp =  gl_TessCoord.x * gl_in[0].gl_Position +
			gl_TessCoord.y * gl_in[1].gl_Position +
			gl_TessCoord.z * gl_in[2].gl_Position;
	pos_tmp = vec4(normalize(pos_tmp.xyz), 1);

	norm = (model_matrix * pos_tmp).xyz;
	light = light_pos - (model_matrix * pos_tmp).xyz;

	gl_Position = model_view_proj_matrix * pos_tmp;
}
