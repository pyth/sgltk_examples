#version 130

in vec3 pos_ls;
in vec3 cam_vec;
in vec3 norm;

out vec4 color;

uniform vec3 light_dir;
uniform sampler2D shadow_map;

void main() {
	float saved_depth = texture(shadow_map, pos_ls.xy).r;
	float shadow = pos_ls.z - 0.01 > saved_depth ? 0.0 : 1.0;
	if(pos_ls.x > 1.0 || pos_ls.y > 1.0)
		shadow = 1.0;

	vec3 cam = normalize(cam_vec);
	vec3 norm = normalize(norm);
	vec3 light = -normalize(light_dir);
	vec3 reflection = normalize(reflect(light, norm));

	float LN = max(0.0, dot(norm, light));
	float VR = max(0.0, dot(cam, reflection) * sign(LN));

	color = 0.2 * vec4(1) + shadow * (LN * vec4(1) + 0.7 * vec4(1) * pow(VR, 10));
}
