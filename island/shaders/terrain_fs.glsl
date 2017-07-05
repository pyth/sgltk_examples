#version 400

in vec2 tc;
in vec3 norm;
in float height;
in vec3 cam_vec;

out vec4 color;

uniform float max_height;
uniform vec3 light_direction;
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

	vec3 tex_water = texture(water_texture, tc).xyz;
	vec3 tex_sand = texture(sand_texture, tc).xyz;
	vec3 tex_grass = texture(grass_texture, tc).xyz;
	vec3 tex_rock = texture(rock_texture, tc).xyz;
	vec3 tex_snow = texture(snow_texture, tc).xyz;

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

	vec4 amb, diff, spec;
	vec3 n = normalize(norm);
	vec3 l = normalize(light_direction);
	vec3 v = normalize(cam_vec);
	float ln = max(0, dot(-l, n));
	float vr = max(0, dot(reflect(l, n), v));
	amb = vec4(0.2 * col, 1);
	diff = vec4(col, 1) * ln;
	if(height > rock_mix ||	height < water_mix)
		spec = vec4(0.3) * pow(vr, 10);
	else
		spec = vec4(0);
	color = amb + diff + spec;
}
