#version 130

in vec2 tc;

out vec4 color;

uniform vec2 center;
uniform float scale;
uniform int iter;

uniform sampler1D textures_ambient;

void main() {
	vec2 z, c;
	c.x = 1.3333 * (tc.x - 0.5) * scale - center.x;
	c.y = (tc.y - 0.5) * scale - center.y;

	int i;
	z = c;
	for(i = 0; i < iter; i++) {
		float x = (pow(z.x, 2) - pow(z.y, 2)) + c.x;
		float y = (z.y * z.x + z.x * z.y) + c.y;

		if((pow(x, 2) + pow(y, 2)) > 4.0)
			break;
		z = vec2(x, y);
	}
	color = texture(textures_ambient, (i == iter ? 0.0 : float(i)) / 100);
}
