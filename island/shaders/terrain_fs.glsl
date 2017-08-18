#version 400

in vec3 pos_w;
in vec4 pos_ls;
in vec2 tc;
in vec3 norm;
in float height;

layout (location = 0) out vec4 color;
layout (location = 1) out float shadow;
layout (location = 2) out vec4 position;
layout (location = 3) out vec3 normals;
layout (location = 4) out float spec;

uniform vec3 cam_pos;
uniform float sand_level;
uniform float sand_mix_level;
uniform float grass_level;
uniform float grass_mix_level;
uniform float rock_level;
uniform float rock_mix_level;
uniform vec3 shadow_distance;
uniform sampler2D sand_texture;
uniform sampler2D grass_texture;
uniform sampler2D rock_texture;
uniform sampler2D snow_texture;
uniform sampler2DShadow shadow_map;

void main() {
	float eta = 0.0001;
	float shadow_fade_dist = 5;

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
	if(height <= rock_level)
		color = mix(color, tex_rock, 1.0f - dot(normalize(norm), vec3(0, 1, 0)));
	color.a = 1;

	float cam_dist = length(cam_pos - pos_w.xyz);
	cam_dist = clamp(1.0 - ((cam_dist - (shadow_distance.x - shadow_fade_dist)) / shadow_fade_dist), 0, 1);
	vec3 pos_shadow = pos_ls.xyz;
	pos_shadow.z -= 0.03;
	shadow = 0.0;
	for(int i = -2; i < 3; i++) {
		for(int j = -2; j < 3; j++) {
			shadow += textureOffset(shadow_map, pos_shadow, ivec2(i, j));
		}
	}
	shadow = (1 - shadow / 25) * cam_dist;
	position = vec4(pos_w, 1);
	normals = normalize(norm);
	if(height > rock_mix_level)
		spec = 1;
	else
		spec = 0;
}
