#version 130

in vec4 pos_ls;
in vec3 cam_vec;
in vec3 norm;
in vec2 tc;

out vec4 color;

uniform vec3 light_dir;
uniform int soft_shadow;
uniform sampler2D shadow_map;
uniform sampler2D texture_diffuse;

void main() {
	float shadow = 0.0;
	vec3 pos = pos_ls.xyz / pos_ls.w * 0.5 + 0.5;
	vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
	for(int x = -soft_shadow; x <= soft_shadow; x++) {
		for(int y = -soft_shadow; y <= soft_shadow; y++) {
			float saved_depth = texture(shadow_map, pos.xy + vec2(x, y) * texel_size).r;
			if(pos.x >= 0.0 && pos.x <= 1.0 &&
					pos.y >= 0.0 && pos.y <= 1.0)
				shadow += pos.z - 0.005 >
					  saved_depth ? 1.0 : 0.0;
		}
	}
	shadow /= pow(soft_shadow * 2 + 1, 2);

	vec4 tex = texture(texture_diffuse, tc);

	vec3 cam = normalize(cam_vec);
	vec3 norm = normalize(norm);
	vec3 light = -normalize(light_dir);
	vec3 reflection = -normalize(reflect(light, norm));

	float LN = max(0.0, dot(norm, light));
	float VR = max(0.0, dot(cam, reflection) * sign(LN));

	vec4 amb = 0.2 * tex;

	vec4 diff = LN * tex;

	vec4 spec = 0.3 * vec4(1) * pow(VR, 10);

	color = amb + (1.0 - shadow) * (diff + spec);
	color = vec4(norm, 1);
}
