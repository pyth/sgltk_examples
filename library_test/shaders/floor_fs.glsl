#version 400

in vec3 cam_vec;
in vec3 tc;
in vec3 light;

out vec4 color;

uniform float time;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normals;
uniform sampler2D texture_lightmap;

float rand(vec2 v) {
	return fract(sin(dot(v, vec2(42.9456, 84.850))) * 50845.8745);
}

void main() {
	vec4 tex = texture(texture_diffuse, tc.xy);
	vec4 sp = texture(texture_specular, tc.xy);
	vec4 bump = texture(texture_normals, tc.xy);
	vec4 lm = texture(texture_lightmap, tc.xy);

	vec3 cam = normalize(cam_vec);
	vec3 norm = normalize(bump.xyz * 2 - vec3(1));
	vec3 l = normalize(light);
	vec3 refl = normalize(reflect(-l, norm));

	float LN = max(0.0, dot(norm, l));
	float VR = max(0.0, dot(refl, cam));

	float light_dist = length(light);
	float attenuation = 1.0 / (1.0 +
					0.05 * light_dist +
					0.0032 * pow(light_dist, 2));

	vec4 amb = vec4(0.2 * tex.xyz, 1);

	vec4 diff = vec4(LN * tex.xyz, 1);

	vec4 spec = 5 * vec4(vec3(sp.x * pow(VR, 10)), 1);

	color = vec4(lm.x * attenuation * (amb + diff + spec).xyz, 1);
}
