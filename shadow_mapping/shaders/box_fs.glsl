#version 130

in vec3 cam_vec;
in vec3 norm;
in vec2 tc;

out vec4 color;

uniform vec3 light_dir;
uniform sampler2D texture_diffuse;

void main() {
	vec4 tex = texture(texture_diffuse, tc);

	vec3 cam = normalize(cam_vec);
	vec3 norm = normalize(norm);
	vec3 light = -normalize(light_dir);
	vec3 reflection = normalize(reflect(light, norm));

	float LN = max(0.0, dot(norm, light));
	float VR = max(0.0, dot(cam, reflection) * sign(LN));

	vec4 amb = 0.2 * tex;

	vec4 diff = LN * tex;

	vec4 spec = vec4(1,0,0,1) * pow(VR, 10);

	color = amb + diff + spec;
}
