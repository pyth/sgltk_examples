#version 130

in vec3 pos;
in vec3 norm;

out vec4 color;

uniform vec3 cam_pos;
uniform samplerCube texture_ambient;

void main() {
	vec3 cam_vec = -normalize(cam_pos - pos);
	vec3 tc = normalize(reflect(cam_vec, normalize(norm)));
	color = texture(texture_ambient, tc);
}
