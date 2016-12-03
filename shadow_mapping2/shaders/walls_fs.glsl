#version 130

in vec3 tc;
in vec3 light;

out vec4 color;

uniform int soft_shadow;
uniform float far_plane;
uniform samplerCube texture_diffuse;
uniform samplerCube shadow_map;

void main() {
	float shadow = 0.0;
	float offset = 0.1;
	for(float x = 0.0; x <= 2 * offset * soft_shadow; x += offset) {
		for(float y = 0.0; y <= 2 * offset * soft_shadow; y += offset) {
			for(float z = 0.0; z <= 2 * offset * soft_shadow; z += offset) {
				float saved_depth = texture(shadow_map, -light + vec3(x, y, z) - vec3(offset * soft_shadow)).r * far_plane;
				float current_depth = length(light);
				shadow += current_depth - 0.01 > saved_depth ? 1.0 : 0.0;
			}
		}
	}
	shadow = shadow / pow(soft_shadow * 2 + 1, 3);

	vec4 tex = texture(texture_diffuse, tc);

	color = 0.2 * tex + (1.0 - shadow) * tex;
}
