#version 400

layout (vertices = 4) out;

in vec3 pos_v_tc[];
in vec3 pos_ts_tc[];
in vec3 tc_tc[];
in vec3 light_ts_tc[];
in vec3 norm_tc[];

out vec3 norm_te[];
out vec3 tc_te[];
out vec3 pos_v_te[];
out vec3 light_ts_te[];

uniform int max_tess_level;

float get_tess_level(float dist0, float dist1) {
	float dist = (dist0 + dist1) / 2.0;

	if(dist <= 20.0)
		return float(max_tess_level);

	else if(dist <= 60.0)
		return float(max_tess_level / 2);

	else if(dist <= 100.0)
		return float(max_tess_level / 4);

	else if(dist <= 180.0)
		return float(max_tess_level / 8);

	else
		return float(max_tess_level / 16);
}

void main(void){
	float dist0 = -pos_v_tc[0].z;
	float dist1 = -pos_v_tc[1].z;
	float dist2 = -pos_v_tc[2].z;
	float dist3 = -pos_v_tc[3].z;

	gl_TessLevelOuter[0] = get_tess_level(dist0, dist3);
	gl_TessLevelOuter[1] = get_tess_level(dist0, dist1);
	gl_TessLevelOuter[2] = get_tess_level(dist1, dist2);
	gl_TessLevelOuter[3] = get_tess_level(dist2, dist3);

	gl_TessLevelInner[0] = gl_TessLevelOuter[1];
	gl_TessLevelInner[1] = gl_TessLevelOuter[0];

	norm_te[gl_InvocationID] = norm_tc[gl_InvocationID];

	tc_te[gl_InvocationID] = tc_tc[gl_InvocationID];

	pos_v_te[gl_InvocationID] = pos_v_tc[gl_InvocationID];

	light_ts_te[gl_InvocationID] = light_ts_tc[gl_InvocationID];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
