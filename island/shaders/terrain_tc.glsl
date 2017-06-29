#version 400

layout (vertices = 4) out;

in vec3 pos_v[];
in vec2 tile_pos_tc[];

out vec2 tc_te[];

uniform uint terrain_side;
uniform int max_tess_level;

float get_tess_level(float dist0, float dist1) {
	float step = 100;
	float dist = mix(dist0, dist1, 0.5);

	if(dist <= step)
		return float(max_tess_level);

	else if(dist <= 2 * step)
		return float(max_tess_level / 2);

	else if(dist <= 4 * step)
		return float(max_tess_level / 4);

	else if(dist <= 8 * step)
		return float(max_tess_level / 8);

	else if(dist <= 16 * step)
		return float(max_tess_level / 16);

	else if(dist <= 32 * step)
		return float(max_tess_level / 32);

	else
		return float(max_tess_level / 64);
}

void main() {
	float dist0 = abs(pos_v[0].z);
	float dist1 = abs(pos_v[1].z);
	float dist2 = abs(pos_v[2].z);
	float dist3 = abs(pos_v[3].z);

	gl_TessLevelOuter[0] = get_tess_level(dist0, dist1);
	gl_TessLevelOuter[1] = get_tess_level(dist0, dist2);
	gl_TessLevelOuter[2] = get_tess_level(dist2, dist3);
	gl_TessLevelOuter[3] = get_tess_level(dist1, dist3);

	gl_TessLevelInner[0] = 0.5 * (gl_TessLevelOuter[1] + gl_TessLevelOuter[3]);
	gl_TessLevelInner[1] = 0.5 * (gl_TessLevelOuter[0] + gl_TessLevelOuter[2]);

	float tc_per_tile = 1.0 / int(terrain_side);
	switch(gl_InvocationID) {
		case 0:
			tc_te[gl_InvocationID] = vec2(tile_pos_tc[0].x * tc_per_tile, tile_pos_tc[0].y * tc_per_tile);
			break;
		case 1:
			tc_te[gl_InvocationID] = vec2((tile_pos_tc[0].x + 1) * tc_per_tile, tile_pos_tc[0].y * tc_per_tile);
			break;
		case 2:
			tc_te[gl_InvocationID] = vec2(tile_pos_tc[0].x * tc_per_tile, (tile_pos_tc[0].y + 1) * tc_per_tile);
			break;
		case 3:
			tc_te[gl_InvocationID] = vec2((tile_pos_tc[0].x + 1) * tc_per_tile, (tile_pos_tc[0].y + 1) * tc_per_tile);
			break;
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
