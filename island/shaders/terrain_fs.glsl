#version 400

in vec3 pos_w;
in vec2 tc;
in vec3 norm;
in float height;

layout (location = 0) out vec4 color;
layout (location = 1) out vec3 normals;
layout (location = 2) out vec3 position;
layout (location = 3) out float spec;

uniform float sand_level;
uniform float sand_mix_level;
uniform float grass_level;
uniform float grass_mix_level;
uniform float rock_level;
uniform float rock_mix_level;
uniform sampler2D sand_texture;
uniform sampler2D grass_texture;
uniform sampler2D rock_texture;
uniform sampler2D snow_texture;
uniform sampler2D shadow_texture;

void main() {
	float eta = 0.0001;

	vec4 tex_sand = texture(sand_texture, tc);
	vec4 tex_grass = texture(grass_texture, tc);
	vec4 tex_rock = texture(rock_texture, tc);
	vec4 tex_snow = texture(snow_texture, tc);

	if(height <= sand_level) {
		color = tex_sand;
	} else if(height <= sand_mix_level || abs(height - sand_level) < eta) {
		color = mix(tex_sand, tex_grass, (height - sand_level) / (sand_mix_level - sand_level));
	} else if(height <= grass_level || abs(height - grass_level) < eta) {
		color = tex_grass;
	} else if(height <= grass_mix_level || abs(height - grass_level) < eta) {
		color = mix(tex_grass, tex_rock, (height - grass_level) / (grass_mix_level - grass_level));
	} else if(height <= rock_level || abs(height - rock_level) < eta) {
		color = tex_rock;
	} else if(height <= rock_mix_level || abs(height - rock_level) < eta) {
		color = mix(tex_rock, tex_snow, (height - rock_level) / (rock_mix_level - rock_level));
	} else {
		color = tex_snow;
	}

	position = pos_w;
	normals = normalize(norm);
	if(height > rock_mix_level)
		spec = 1;
	else
		spec = 0;
}
