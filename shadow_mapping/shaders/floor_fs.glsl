#version 130

in vec3 pos_ls;
in vec3 cam_vec;
in vec3 light_vec;
in vec3 norm;
in vec2 tc;

out vec4 color;

uniform sampler2D shadow_map;

void main() {
	float shadow = texture(shadow_map, pos_ls.xy).x;

	vec3 cam = normalize(cam_vec);
	vec3 norm = normalize(norm);
	vec3 light = normalize(light_vec);
	vec3 reflection = normalize(reflect(light, norm));

	float LN = max(0.0, dot(norm, light));
	float VR = max(0.0, dot(cam, reflection) * sign(LN));

	color = 0.2 * vec4(1) + LN * vec4(1) + 0.7 * vec4(1) * pow(VR, 10);
	color = 0.2 * vec4(1) + shadow * LN * vec4(1);
}
