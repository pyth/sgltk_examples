#version 400

in vec3 pos_w;
in vec2 tc;
in vec3 norm;
in float height;
in vec4 pos_ls[3];

layout (location = 0) out vec4 color;

uniform vec3 cam_pos;
uniform vec3 light_direction;
uniform float max_height;
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
uniform sampler2D shadow_texture;
uniform sampler2DShadow shadow_map_near;
uniform sampler2DShadow shadow_map_mid;
uniform sampler2DShadow shadow_map_far;

void main() {
	vec3 col;
	float eta = 0.0001;
	float shadow_fade_dist = 5;

	vec3 tex_sand = texture(sand_texture, tc).xyz;
	vec3 tex_grass = texture(grass_texture, tc).xyz;
	vec3 tex_rock = texture(rock_texture, tc).xyz;
	vec3 tex_snow = texture(snow_texture, tc).xyz;

	if(height <= sand_level) {
		col = tex_sand;
	} else if(height <= sand_mix_level || abs(height - sand_level) < eta) {
		col = mix(tex_sand, tex_grass, (height - sand_level) / (sand_mix_level - sand_level));
	} else if(height <= grass_level || abs(height - grass_level) < eta) {
		col = tex_grass;
	} else if(height <= grass_mix_level || abs(height - grass_level) < eta) {
		col = mix(tex_grass, tex_rock, (height - grass_level) / (grass_mix_level - grass_level));
	} else if(height <= rock_level || abs(height - rock_level) < eta) {
		col = tex_rock;
	} else if(height <= rock_mix_level || abs(height - rock_level) < eta) {
		col = mix(tex_rock, tex_snow, (height - rock_level) / (rock_mix_level - rock_level));
	} else {
		col = tex_snow;
	}
	if(height <= rock_level)
		col = mix(col, tex_rock, 1.0f - dot(normalize(norm), vec3(0, 1, 0)));

	float cam_dist = length(cam_pos - pos_w.xyz);

	float cam_dist0 = clamp(1.0 - ((cam_dist - (shadow_distance.x - shadow_fade_dist)) / shadow_fade_dist), 0, 1);
	vec3 pos_shadow0 = pos_ls[0].xyz;
	pos_shadow0.z -= 0.03;

	float shadow = 0.0;
	shadow += textureOffset(shadow_map_near, pos_shadow0, ivec2(-1, -1));
	shadow += textureOffset(shadow_map_near, pos_shadow0, ivec2(-1, 0));
	shadow += textureOffset(shadow_map_near, pos_shadow0, ivec2(-1, 1));
	shadow += textureOffset(shadow_map_near, pos_shadow0, ivec2(0, -1));
	shadow += textureOffset(shadow_map_near, pos_shadow0, ivec2(0, 0));
	shadow += textureOffset(shadow_map_near, pos_shadow0, ivec2(0, 1));
	shadow += textureOffset(shadow_map_near, pos_shadow0, ivec2(1, -1));
	shadow += textureOffset(shadow_map_near, pos_shadow0, ivec2(1, 0));
	shadow += textureOffset(shadow_map_near, pos_shadow0, ivec2(1, 1));
	shadow = (1 - shadow / 9) * cam_dist0;

	vec3 v = normalize(cam_pos - pos_w);
	vec3 light = normalize(light_direction);
	float vr = max(0, dot(reflect(light, normalize(norm)), v));
	vec4 amb = vec4(0.2 * col, 1);
	vec4 diff = vec4(max(0, dot(normalize(norm), -light)) * col, 1);
	vec4 spec = vec4(0, 0, 0, 1);
	if(height >= rock_mix_level)
		spec = vec4(0.3) * pow(vr, 10);
	color = amb + (1 - shadow) * (diff + spec);
	color.a = spec.r;
}
