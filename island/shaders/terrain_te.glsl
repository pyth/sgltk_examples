#version 400

layout (quads, equal_spacing, cw) in;

in vec2 tc_te[];

out vec2 tc;
out vec3 norm;
out float height;

uniform sampler2D texture_displacement;
uniform float max_height;
uniform mat4 view_proj_matrix;
uniform uint terrain_side;
uniform int max_tess_level;

float get_height(vec2 coordinates) {
	return max_height * texture(texture_displacement, coordinates).x;
}

float get_height(vec2 coordinates, ivec2 offset) {
	return max_height * textureOffset(texture_displacement, coordinates, offset).x;
}

vec3 get_normal(vec2 coordinates) {
	vec3 v[8];
	int index = 0;
	vec3 normal = vec3(0, 0, 0);
	vec3 v0 = vec3(0, get_height(coordinates), 0);
	for(int i = -1; i <= 1; i++) {
		for(int j = -1; j <= 1; j++) {
			if(i == 0 && j == 0)
				continue;
			ivec2 offset = ivec2(i, j);
			v[index++] = vec3(i, get_height(coordinates, offset), j);
		}
	}
	normal += cross(v[0] - v0, v[1] - v0);
	normal += cross(v[1] - v0, v[2] - v0);
	normal += cross(v[2] - v0, v[4] - v0);
	normal += cross(v[4] - v0, v[7] - v0);
	normal += cross(v[7] - v0, v[6] - v0);
	normal += cross(v[6] - v0, v[5] - v0);
	normal += cross(v[5] - v0, v[3] - v0);
	normal += cross(v[3] - v0, v[0] - v0);
	return normal;
}

void main() {
	vec2 tc0 = mix(tc_te[0], tc_te[1], gl_TessCoord.x);
	vec2 tc1 = mix(tc_te[2], tc_te[3], gl_TessCoord.x);
	tc = mix(tc0, tc1, gl_TessCoord.y);

	norm = get_normal(tc);

	height = get_height(tc);

	vec4 pos0 = mix(gl_in[0].gl_Position, gl_in[2].gl_Position, gl_TessCoord.x);
	vec4 pos1 = mix(gl_in[1].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 pos = mix(pos0, pos1, gl_TessCoord.y);
	pos.y += height;
	gl_Position = view_proj_matrix *  pos;
}
