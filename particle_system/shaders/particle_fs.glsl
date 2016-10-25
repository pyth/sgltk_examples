#version 130

in float life;

out vec4 color;

void main() {
	color = vec4(life, 0, 1 - life, 0);
}
