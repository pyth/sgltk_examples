#version 330

in vec3 cam_vec;
in vec3 pos_w;
in vec4 pos;
in vec2 tc;
in vec4 pos_ls[3];

layout (location = 0) out vec4 color;
layout (location = 1) out float shadow;
layout (location = 2) out vec4 position;
layout (location = 4) out vec3 normals;
layout (location = 5) out float spec;

uniform vec3 cam_pos;
uniform vec2 near_far;
uniform vec3 light_direction;
uniform vec3 shadow_distance;
uniform sampler2D depth_texture;
uniform sampler2D water_dudv_texture;
uniform sampler2D refraction_texture;
uniform sampler2D reflection_texture;
uniform sampler2DArrayShadow shadow_map;

float lin_depth(in float depth, in float near, in float far) {
	float d = 2.0 * depth - 1.0;
	return (2.0 * near * far) / (far + near - d * (far - near)) / far;
}

void main() {
	float shadow_fade_dist = 5;
	float distortion_factor = 0.007;

	vec2 tex_coord = (pos.xy / pos.w + vec2(1)) / 2;
	float depth = texture(depth_texture, tex_coord).r;
	float dist1 = lin_depth(depth, near_far.x, near_far.y);
	float dist2 = lin_depth(gl_FragCoord.z, near_far.x, near_far.y);
	float water_depth = clamp((dist1 - dist2) * 100, 0.0, 1.0);
	vec2 distortion1 = texture(water_dudv_texture, vec2(tc.x, 1.0 - tc.y)).rg;
	vec2 distortion2 = texture(water_dudv_texture, vec2(tc.x, tc.y)).rg;
	vec2 distortion = water_depth * distortion_factor * (mix(distortion1, distortion2, 0.5) * 2 - vec2(1));

	vec2 refr_coord = vec2(tex_coord.x, tex_coord.y);
	refr_coord += distortion;
	refr_coord = clamp(refr_coord, 0, 1);
	vec2 refl_coord = vec2(tex_coord.x, 1.0 - tex_coord.y);
	refl_coord += distortion;
	refl_coord = clamp(refl_coord, 0, 1);
	vec4 refr_tex = texture(refraction_texture, refr_coord);
	vec4 refl_tex = texture(reflection_texture, refl_coord);

	vec4 refr = vec4(refr_tex.rgb, 1);
	vec4 refl = vec4(refl_tex.rgb, 1);

	vec3 cam_vec = cam_pos - pos_w;
	vec3 n = normalize(vec3(0, 1, 0) + vec3(distortion.x, 0, distortion.y) * water_depth);
	float vr = max(0, dot(reflect(-normalize(light_direction), n), -normalize(cam_vec)));
	float fresnell = clamp(pow(dot(normalize(cam_vec), n), 0.5), 0, 1);
	vec3 sp = vec3(0.4) * pow(vr, 10) * fresnell;

	refr = mix(refr, vec4(0, 0.5, 0.5, 1), 0.1);
	refl = mix(refl, vec4(0, 0.5, 0.5, 1), 0.1);
	vec4 refl_refr = mix(refl, refr, fresnell * (1 - refr_tex.a));

	float cam_dist = length(cam_pos - pos_w.xyz);

	vec4 pos_shadow0 = vec4(pos_ls[0].xy, 0, pos_ls[0].z);
	pos_shadow0 += vec4(distortion * 0.1, 0, -0.035);

	vec4 pos_shadow1 = vec4(pos_ls[1].xy, 1, pos_ls[1].z);
	pos_shadow1 += vec4(distortion * 0.1, 0, -0.01);

	vec4 pos_shadow2 = vec4(pos_ls[2].xy, 2, pos_ls[2].z);
	pos_shadow2 += vec4(distortion * 0.1, 0, -0.002);

	shadow = 0.0;
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

	refl_refr *= max(shadow, 0.9);
	sp *= shadow;

	color = vec4(refl_refr.rgb + sp, 1);
	shadow = 1.0;
	position = vec4(pos_w, 0);
	normals = vec3(0, 1, 0);
	spec = mix(refr_tex.a, refl_tex.a, 0.5);
}
