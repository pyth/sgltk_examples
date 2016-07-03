#version 400

layout (vertices = 4) out;

in vec3 pos_v_tc[];
in vec3 pos_ts_tc[];
in vec3 tc_tc[];
in vec3 light_ts_tc[];

out vec3 tc_te[];
out vec3 pos_ts_te[];
out vec3 light_ts_te[];

float get_tess_level(float dist0, float dist1) {
	float dist = (dist0 + dist1) / 2.0;

	if(dist <= 20.0)
		return 64.0;

	else if(dist <= 40.0)
		return 32.0;

	else if(dist <= 80.0)
		return 16.0;

	else if(dist <= 160.0)
		return 8.0;

	else
		return 2.0;
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

	tc_te[gl_InvocationID] = tc_tc[gl_InvocationID];

	pos_ts_te[gl_InvocationID] = pos_ts_tc[gl_InvocationID];

	light_ts_te[gl_InvocationID] = light_ts_tc[gl_InvocationID];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
