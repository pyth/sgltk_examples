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
uniform sampler2D shadow_texture;
uniform sampler2DArray terrain_texture;
uniform sampler2DArrayShadow shadow_map;

void main() {
	vec3 col;
	float eta = 0.0001;
	float shadow_fade_dist = 5;

	vec3 tex_sand = texture(terrain_texture, vec3(tc, 0)).xyz;
	vec3 tex_grass = texture(terrain_texture, vec3(tc, 1)).xyz;
	vec3 tex_rock = texture(terrain_texture, vec3(tc, 2)).xyz;
	vec3 tex_snow = texture(terrain_texture, vec3(tc, 3)).xyz;

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

	vec4 pos_shadow0 = vec4(pos_ls[0].xy, 0, pos_ls[0].z);
	pos_shadow0.w -= 0.035;

	vec4 pos_shadow1 = vec4(pos_ls[1].xy, 1, pos_ls[1].z);
	pos_shadow1.w -= 0.01;

	vec4 pos_shadow2 = vec4(pos_ls[2].xy, 2, pos_ls[2].z);
	pos_shadow2.w -= 0.002;

	float shadow = 0.0;
	vec2 texel_size = 1.0 / textureSize(shadow_map, 0).xy;
	for(int x = -1; x < 2; x++) {
		for(int y = -1; y < 2; y++) {
			if(cam_dist <= shadow_distance.x) {
				//color = vec4(1,0,0,1);
				shadow += texture(shadow_map, pos_shadow0 + vec4(vec2(x, y) * texel_size, 0, 0));
			} else if(cam_dist <= shadow_distance.y) {
				//color = vec4(0,1,0,1);
				shadow += texture(shadow_map, pos_shadow1 + vec4(vec2(x, y) * texel_size, 0, 0));
			} else {
				//color = vec4(0,0,1,1);
				shadow += texture(shadow_map, pos_shadow2 + vec4(vec2(x, y) * texel_size, 0, 0));
			}
		}
	}
	shadow = clamp(shadow / 9, 0, 1);

	vec3 v = normalize(cam_pos - pos_w);
	vec3 light = normalize(light_direction);
	float vr = max(0, dot(reflect(light, normalize(norm)), v));
	vec4 amb = vec4(0.2 * col, 1);
	vec4 diff = vec4(max(0, dot(normalize(norm), -light)) * col, 1);
	vec4 spec = vec4(0, 0, 0, 1);
	if(height >= rock_mix_level)
		spec = vec4(0.3) * pow(vr, 10);
	color = amb + shadow * (diff + spec);
	color.a = spec.r;
}
