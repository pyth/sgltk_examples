#version 130

in vec3 cam_vec;
in vec3 norm;

out vec4 color;

uniform samplerCube texture_ambient;

void main() {
	vec3 n = normalize(norm);
	float CR = clamp(dot(cam_vec, n), 0.0, 1.0);

	vec3 tc_refr = normalize(refract(cam_vec, n, 0.8));
	vec3 tc_refl = -normalize(reflect(cam_vec, n));

	vec4 refr = texture(texture_ambient, tc_refr);
	vec4 refl = texture(texture_ambient, tc_refl);

	color = mix(refl, refr, CR);
}
