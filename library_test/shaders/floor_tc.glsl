#version 400

layout (vertices = 4) out;

in vec3 cam_vec_tc[];
in vec3 pos_v_tc[];
in vec3 pos_ts_tc[];
in vec3 tc_tc[];
in vec3 light_tc[];

out vec3 cam_vec_te[];
out vec3 tc_te[];
out vec3 light_te[];

uniform int max_tess_level;

float get_tess_level(float dist0, float dist1) {
	float step = 20;
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

void main(void){
	float dist0 = abs(pos_v_tc[0].z);
	float dist1 = abs(pos_v_tc[1].z);
	float dist2 = abs(pos_v_tc[2].z);
	float dist3 = abs(pos_v_tc[3].z);

	gl_TessLevelOuter[0] = get_tess_level(dist0, dist3);
	gl_TessLevelOuter[1] = get_tess_level(dist0, dist1);
	gl_TessLevelOuter[2] = get_tess_level(dist1, dist2);
	gl_TessLevelOuter[3] = get_tess_level(dist2, dist3);

	gl_TessLevelInner[0] = gl_TessLevelOuter[1];
	gl_TessLevelInner[1] = gl_TessLevelOuter[0];

	cam_vec_te[gl_InvocationID] = cam_vec_tc[gl_InvocationID];

	tc_te[gl_InvocationID] = tc_tc[gl_InvocationID];

	light_te[gl_InvocationID] = light_tc[gl_InvocationID];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
