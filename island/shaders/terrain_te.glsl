#version 400

layout (quads, equal_spacing, cw) in;

in vec2 tc_te[];
in vec2 tc2_te[];

out vec3 pos_w;
out vec2 tc;
out vec3 norm;
out float height;
out vec4 pos_ls[3];

uniform vec4 clip_plane;
uniform mat4 light_matrix[3];
uniform sampler2D texture_displacement;
uniform float max_height;
uniform mat4 view_matrix;
uniform mat4 view_proj_matrix;
uniform uint terrain_side;
uniform int max_tess_level;

float get_height(vec2 coordinates) {
	return max_height * texture(texture_displacement, coordinates).x;
}

vec3 get_normal(vec2 coordinates) {
	vec3 normal = vec3(0, 0, 0);
	vec3 v0 = vec3(0, max_height * texture(texture_displacement, coordinates).x, 0);
	vec3 v1 = vec3(-1, max_height * textureOffset(texture_displacement, coordinates, ivec2(-1, -1)).x, -1);
	vec3 v2 = vec3(-1, max_height * textureOffset(texture_displacement, coordinates, ivec2(-1, 0)).x, 0);
	vec3 v3 = vec3(-1, max_height * textureOffset(texture_displacement, coordinates, ivec2(-1, 1)).x, 1);
	vec3 v4 = vec3(0, max_height * textureOffset(texture_displacement, coordinates, ivec2(0, -1)).x, -1);
	vec3 v5 = vec3(0, max_height * textureOffset(texture_displacement, coordinates, ivec2(0, 1)).x, 1);
	vec3 v6 = vec3(1, max_height * textureOffset(texture_displacement, coordinates, ivec2(1, -1)).x, -1);
	vec3 v7 = vec3(1, max_height * textureOffset(texture_displacement, coordinates, ivec2(1, 0)).x, 0);
	vec3 v8 = vec3(1, max_height * textureOffset(texture_displacement, coordinates, ivec2(1, 1)).x, 1);
	normal += cross(v1 - v0, v2 - v0);
	normal += cross(v2 - v0, v3 - v0);
	normal += cross(v3 - v0, v5 - v0);
	normal += cross(v5 - v0, v8 - v0);
	normal += cross(v8 - v0, v7 - v0);
	normal += cross(v7 - v0, v6 - v0);
	normal += cross(v6 - v0, v4 - v0);
	normal += cross(v4 - v0, v1 - v0);
	return normal;
}

void main() {
	vec2 tc0 = mix(tc_te[0], tc_te[1], gl_TessCoord.x);
	vec2 tc1 = mix(tc_te[2], tc_te[3], gl_TessCoord.x);
	vec2 tc_height = mix(tc0, tc1, gl_TessCoord.y);

	tc0 = mix(tc2_te[0], tc2_te[1], gl_TessCoord.x);
	tc1 = mix(tc2_te[2], tc2_te[3], gl_TessCoord.x);
	tc = mix(tc0, tc1, gl_TessCoord.y) * 4;

	norm = get_normal(tc_height);

	height = get_height(tc_height);

	vec4 pos0 = mix(gl_in[0].gl_Position, gl_in[2].gl_Position, gl_TessCoord.x);
	vec4 pos1 = mix(gl_in[1].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 pos = mix(pos0, pos1, gl_TessCoord.y);
	pos.y += height;
	pos_w = pos.xyz;

	for(int i = 0; i < 3; i++) {
		pos_ls[i] = 0.5f * (light_matrix[i] * pos) + vec4(vec3(0.5), 0);
	}

	gl_Position = view_proj_matrix *  pos;

	gl_ClipDistance[0] = dot(pos, clip_plane);
}
