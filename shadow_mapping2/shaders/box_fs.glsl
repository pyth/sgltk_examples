#version 130

in vec3 cam_vec;
in vec3 norm;
in vec2 tc;
in vec3 light;

out vec4 color;

uniform int soft_shadow;
uniform float far_plane;
uniform samplerCube shadow_map;
uniform sampler2D texture_diffuse;

void main() {
	float shadow = 0.0;
	float offset = 0.1;
	for(float x = 0.0; x <= 2 * offset * soft_shadow; x += offset) {
		for(float y = 0.0; y <= 2 * offset * soft_shadow; y += offset) {
			for(float z = 0.0; z <= 2 * offset * soft_shadow; z += offset) {
				float saved_depth = texture(shadow_map, -light + vec3(x, y, z) - vec3(offset * soft_shadow)).r * far_plane;
				float current_depth = length(light);
				shadow += current_depth - 0.001 > saved_depth ? 1.0 : 0.0;
			}
		}
	}
	shadow = shadow / pow(soft_shadow * 2 + 1, 3);

	vec4 tex = texture(texture_diffuse, tc);

	vec3 cam = normalize(cam_vec);
	vec3 n = normalize(norm);
	vec3 l = normalize(light);
	vec3 reflection = -normalize(reflect(l, n));

	float LN = max(0.0, dot(n, l));
	float VR = max(0.0, dot(cam, reflection) * sign(LN));

	vec4 amb = 0.2 * tex;

	vec4 diff = LN * tex;

	vec4 spec = 0.3 * vec4(1) * pow(VR, 10);

	color = amb + (1.0 - shadow) * (diff + spec);
}
