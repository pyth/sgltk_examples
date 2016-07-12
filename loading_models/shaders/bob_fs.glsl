#version 130

in vec3 cam_vec;
in vec3 tc;
in vec3 light_vec;

out vec4 color;

uniform float time;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normals;

void main() {
	vec4 tex = texture(texture_diffuse, tc.xy);
	vec4 sp = texture(texture_specular, tc.xy);
	vec4 bump = texture(texture_normals, tc.xy);

	vec3 norm = normalize(bump.xyz * 2 - vec3(1));
	vec3 light = normalize(light_vec);
	vec3 reflection = normalize(reflect(-light, norm));

	float LN = max(0.0, dot(norm, light));
	float VR = abs(max(0.0, dot(normalize(cam_vec), reflection)));
	VR *= sign(LN);

	float light_dist = length(light_vec);
	float attenuation = 1.0 / (1.0 +
					0.09 * light_dist +
					0.0032 * light_dist * light_dist);

	vec4 amb = vec4(0.2 * tex.xyz, 1);

	vec4 diff = vec4(LN * tex.xyz, 1);

	vec4 spec = vec4(sp.xyz * pow(VR, 10), 1);

	color = vec4(attenuation * (amb + diff + spec).xyz, 1);
}
