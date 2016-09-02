#version 330

in vec4 pos;
in vec3 norm;

out vec4 color;

uniform vec4 cam_pos;
uniform samplerCube texture_ambient;

void main() {
	vec3 cam_vec = -normalize(cam_pos - pos).xyz;
	vec3 tc = normalize(reflect(cam_vec, normalize(norm)));
	color = texture(texture_ambient, tc);
}
