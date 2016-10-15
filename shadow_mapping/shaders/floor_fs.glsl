#version 130

in vec3 pos_ls;
in vec3 cam_vec;
in vec3 norm;
in vec2 tc;

out vec4 color;

uniform vec3 light_dir;
uniform int soft_shadow;
uniform sampler2D texture_diffuse;
uniform sampler2D shadow_map;

void main() {
	float shadow = 0.0;
	vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
	for(int x = -soft_shadow; x <= soft_shadow; x++) {
		for(int y = -soft_shadow; y <= soft_shadow; y++) {
			float saved_depth = texture(shadow_map, pos_ls.xy + vec2(x, y) * texel_size).r;
			if(pos_ls.x > 1.0 || pos_ls.y > 1.0)
				shadow += 0.0;
			else
				shadow += pos_ls.z - 0.001 > saved_depth ? 1.0 : 0.0;
		}
	}
	shadow /= pow(soft_shadow * 2 + 1, 2);

	vec4 tex = texture(texture_diffuse, tc);

	vec3 cam = normalize(cam_vec);
	vec3 norm = normalize(norm);
	vec3 light = -normalize(light_dir);
	vec3 reflection = normalize(reflect(light, norm));

	float LN = max(0.0, dot(norm, light));
	float VR = max(0.0, dot(cam, reflection) * sign(LN));

	vec4 amb = 0.2 * tex;
	vec4 diff = LN * tex;
	vec4 spec = 0.7 * tex * pow(VR, 10);

	color = amb + (1.0 - shadow) * (diff + spec);
}
