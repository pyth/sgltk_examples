#version 400

in vec3 pos_w;
in vec2 tc;
in vec2 tc2;
in vec3 norm;
in float height;

layout (location = 0) out vec4 color;
layout (location = 1) out vec3 normals;
layout (location = 2) out vec3 position;

uniform float max_height;
uniform sampler2D water_texture;
uniform sampler2D sand_texture;
uniform sampler2D grass_texture;
uniform sampler2D rock_texture;
uniform sampler2D snow_texture;

void main() {
	vec3 col;
	float eta = 0.0001;
	float water_height = 0.1 * max_height;
	float water_mix = 0.15 * max_height;
	float sand_height = 0.2 * max_height;
	float sand_mix = 0.25 * max_height;
	float grass_height = 0.3 * max_height;
	float grass_mix = 0.35 * max_height;
	float rock_height = 0.4 * max_height;
	float rock_mix = 0.45 * max_height;

	vec3 tex_water = texture(water_texture, tc2).xyz;
	vec3 tex_sand = texture(sand_texture, tc2).xyz;
	vec3 tex_grass = texture(grass_texture, tc2).xyz;
	vec3 tex_rock = texture(rock_texture, tc2).xyz;
	vec3 tex_snow = texture(snow_texture, tc2).xyz;

	if(height >= 0) {
		col = tex_water;
	} if(height > water_height || abs(height - water_height) < eta) {
		col = mix(tex_water, tex_sand, (height - water_height) / (water_mix - water_height));
	} if(height > water_mix || abs(height - water_mix) < eta) {
		col = tex_sand;
	} if(height > sand_height || abs(height - sand_height) < eta) {
		col = mix(tex_sand, tex_grass, (height - sand_height) / (sand_mix - sand_height));
	} if(height > sand_mix || abs(height - sand_mix) < eta) {
		col = tex_grass;
	} if(height > grass_height || abs(height - grass_height) < eta) {
		col = mix(tex_grass, tex_rock, (height - grass_height) / (grass_mix - grass_height));
	} if(height > grass_mix || abs(height - grass_mix) < eta) {
		col = tex_rock;
	} if(height > rock_height || abs(height - rock_height) < eta) {
		col = mix(tex_rock, tex_snow, (height - rock_height) / (rock_mix - rock_height));
	} if(height > rock_mix || abs(height - rock_mix) < eta) {
		col = tex_snow;
	}

	position = pos_w;
	normals = normalize(norm);
	color.rgb = col;
	if(height > rock_mix ||	height < water_mix)
		color.a = 1;
	else
		color.a = 0;
}
