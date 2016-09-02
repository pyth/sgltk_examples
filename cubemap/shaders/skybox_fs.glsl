#version 330

in vec3 tc;

out vec4 color;

uniform samplerCube texture_ambient;

void main() {
	color = texture(texture_ambient, tc);
}
