#version 400

in vec2 tc;
in vec3 norm;
in float height;

out vec4 color;

uniform float max_height;
uniform vec3 light_direction;

void main() {
	vec3 col;
	if(height >= 0) {
		col = vec3(0, 0, 1);
	} if(height > 0.1 * max_height) {
		col = vec3(0, 1, 0);
	} if(height > 0.3 * max_height) {
		col = vec3(0.3, 0.2, 0.2);
	} if(height > 0.4 * max_height) {
		col = vec3(1.0, 1.0, 1.0);
	}

	vec4 amb, diff;
	vec3 n = normalize(norm);
	vec3 l = normalize(light_direction);
	amb = vec4(0.2 * col, 1);
	if(height >= 0) {
		diff = vec4(max(0, dot(-l, n)) * col, 1);
	} else {
		diff = vec4(col, 1);
	}
	color = amb + diff;
	//color = vec4(n, 1);
}
