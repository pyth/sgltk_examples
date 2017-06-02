#version 400

in vec3 light;
in vec3 norm;
out vec4 color;

void main() {
	vec3 n = normalize(norm);
	vec3 l = normalize(light);
	vec4 amb = 0.2 * vec4(1);
	vec4 diff = 0.7 * max(0, dot(n, l)) * vec4(1);
	color = amb + diff;
}
