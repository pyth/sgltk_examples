#version 400

in vec2 tc;

out vec4 color;

uniform vec2 screen_res;
uniform dvec2 center;
uniform double scale;
uniform int iter;

uniform sampler1D textures_ambient;

void main() {
	uint i;
	dvec2 coord = tc;
	dvec2 c = dvec2(screen_res.x / screen_res.y * (coord.x - 0.5) * scale - center.x,
							(coord.y - 0.5) * scale - center.y);
	dvec2 z = c;
	for(i = 0; i < iter; i++) {
		z = dvec2(z.x * z.x - z.y * z.y + c.x, z.y * z.x + z.x * z.y + c.y);

		if(z.x * z.x + z.y * z.y > 4.0)
			break;
	}
	color = texture(textures_ambient, (i == iter ? 0.0 : float(i)) / 100);
}
