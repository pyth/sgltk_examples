#version 130

in vec4 pos_eye;
in vec4 norm;
in vec4 light_vec;

out vec4 color;

uniform vec4 color_ambient;
uniform vec4 color_diffuse;
uniform vec4 color_specular;
uniform float shininess;
uniform float shininess_strength;

void main() {
	vec3 eye = normalize(-pos_eye.xyz);
	vec3 norm = normalize(norm.xyz);
	vec3 light = normalize(light_vec.xyz);
	vec3 reflection = -normalize(reflect(light, norm));

	float LN = max(0.0, dot(norm, light));
	float VR = max(0.0, dot(eye, reflection) * sign(LN));

	float light_dist = length(light_vec);
	float attenuation = 1.0 / (1.0 +
					0.09 * light_dist +
					0.0032 * light_dist * light_dist);

	vec4 amb = color_ambient;
	if(amb.xyz == vec3(0.0))
		amb = vec4(0.1 * color_diffuse.xyz, 1.0);

	vec4 diff = vec4(LN * color_diffuse.xyz,
			color_diffuse.w);

	vec4 spec = color_specular *
			shininess_strength *
			pow(VR, shininess);

	color = amb + vec4(attenuation * (diff + spec).xyz, 1);
}
