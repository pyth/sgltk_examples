#version 130

in vec3 cam_vec;
in vec3 norm;
in vec2 tc;
in vec4 pos_w;

out vec4 color;

uniform vec3 light_pos;
uniform int soft_shadow;
uniform float far_plane;
uniform sampler2D texture_diffuse;
uniform samplerCube shadow_map;

void main() {
	vec3 light = light_pos - pos_w.xyz;
	float shadow = 0.0;
	float saved_depth = texture(shadow_map, light).r * far_plane;
	float current_depth = length(light);
	shadow = current_depth - 0.005 > saved_depth ? 1.0 : 0.0;

	vec4 tex = texture(texture_diffuse, tc);

	vec3 cam = normalize(cam_vec);
	vec3 n = normalize(norm);
	vec3 l = normalize(light);
	vec3 reflection = normalize(reflect(l, n));

	float LN = max(0.0, dot(norm, l));
	float VR = max(0.0, dot(cam, reflection) * sign(LN));

	vec4 amb = 0.2 * tex;
	vec4 diff = LN * tex;
	vec4 spec = 0.7 * tex * pow(VR, 10);

	color = amb + (1.0 - shadow) * (diff + spec);
}
