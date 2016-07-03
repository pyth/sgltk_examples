#version 400

layout (quads, equal_spacing, ccw) in;

in vec3 pos_ts_te[];
in vec3 light_ts_te[];
in vec3 tc_te[];

out vec3 tc;
out vec3 pos_ts;
out vec3 light_ts;

uniform sampler2D texture_displacement;
uniform mat4 view_proj_matrix;

void main(void){
	vec3 tc0 = mix(tc_te[0], tc_te[1], gl_TessCoord.x);
	vec3 tc1 = mix(tc_te[2], tc_te[3], gl_TessCoord.x);
	tc = mix(tc0, tc1, gl_TessCoord.y);

	float height = texture(texture_displacement, tc.xy).x;
	vec4 pos0 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 pos1 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 pos = mix(pos0, pos1, gl_TessCoord.y);
	pos.y += height * 0.1 - 0.1;

	vec3 pos_v0 = mix(pos_ts_te[0], pos_ts_te[1], gl_TessCoord.x);
	vec3 pos_v1 = mix(pos_ts_te[2], pos_ts_te[3], gl_TessCoord.x);
	pos_ts = mix(pos_v0, pos_v1, gl_TessCoord.y);

	vec3 light0 = mix(light_ts_te[0], light_ts_te[1], gl_TessCoord.x);
	vec3 light1 = mix(light_ts_te[2], light_ts_te[3], gl_TessCoord.x);
	light_ts = mix(light0, light1, gl_TessCoord.y);

	gl_Position = view_proj_matrix * pos;
}
