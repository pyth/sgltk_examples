#version 400

layout (quads, equal_spacing, ccw) in;

in vec3 cam_vec_te[];
in vec3 light_te[];
in vec3 tc_te[];

out vec3 tc;
out vec3 cam_vec;
out vec3 light;

uniform sampler2D texture_displacement;
uniform mat4 view_proj_matrix;

void main() {
	vec3 tc0 = mix(tc_te[0], tc_te[1], gl_TessCoord.x);
	vec3 tc1 = mix(tc_te[2], tc_te[3], gl_TessCoord.x);
	tc = mix(tc0, tc1, gl_TessCoord.y);

	float height = texture(texture_displacement, tc.xy).x;

	vec4 pos0 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 pos1 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 pos = mix(pos0, pos1, gl_TessCoord.y);
	pos.y += height * 0.2 - 0.2;

	vec3 light0 = mix(light_te[0], light_te[1], gl_TessCoord.x);
	vec3 light1 = mix(light_te[2], light_te[3], gl_TessCoord.x);
	light = mix(light0, light1, gl_TessCoord.y);

	vec3 cam0 = mix(cam_vec_te[0], cam_vec_te[1], gl_TessCoord.x);
	vec3 cam1 = mix(cam_vec_te[2], cam_vec_te[3], gl_TessCoord.x);
	cam_vec = mix(cam0, cam1, gl_TessCoord.y);

	gl_Position = view_proj_matrix * pos;
}
