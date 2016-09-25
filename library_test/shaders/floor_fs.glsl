#version 400

in vec3 pos_v;
in vec3 tc;
in vec3 light_ts;

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

	vec3 cam = normalize(-pos_v);
	vec3 norm = normalize(bump.xyz * 2 - vec3(1));
	vec3 light = normalize(light_ts);
	vec3 refl = -normalize(reflect(light, norm));

	float LN = max(0.0, dot(norm, light));
	float VR = max(0.0, dot(refl, cam));

	float light_dist = length(light_ts);
	float attenuation = 1.0 / (1.0 +
					0.09 * light_dist +
					0.032 * light_dist * light_dist);

	vec4 amb = vec4(0.2 * tex.xyz, 1);

	vec4 diff = vec4(LN * tex.xyz, 1);

	vec4 spec = sp * pow(VR, 10);

	color = vec4(lm.x * attenuation * (amb + diff + spec).xyz, 1);
	//color = spec;
	//color = vec4(vec3(attenuation), 1);
}
