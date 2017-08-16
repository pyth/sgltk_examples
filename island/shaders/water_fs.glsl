#version 330

in vec3 cam_vec;
in vec3 pos_w;
in vec4 pos_ls;
in vec4 pos;
in vec2 tc;

layout (location = 0) out vec4 color;
layout (location = 1) out vec3 normals;
layout (location = 2) out vec3 position;
layout (location = 3) out vec4 position_ls;

uniform vec3 cam_pos;
uniform vec2 near_far;
uniform vec3 light_direction;
uniform sampler2D depth_texture;
//uniform sampler2D water_texture;
uniform sampler2D water_dudv_texture;
uniform sampler2D refraction_texture;
uniform sampler2D reflection_texture;

float lin_depth(in float depth, in float near, in float far) {
	float d = 2.0 * depth - 1.0;
	return (2.0 * near * far) / (far + near - d * (far - near)) / far;
}

void main() {
	float distortion_factor = 0.02;
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
	vec3 refr_tex = texture(refraction_texture, refr_coord).rgb;
	vec3 refl_tex = texture(reflection_texture, refl_coord).rgb;

	//vec4 water_tex = texture(water_texture, tc);

	vec4 refr = vec4(refr_tex, 1);
	vec4 refl = vec4(refl_tex, 1);

	normals = vec3(0, 1, 0);
	position = pos_w;
	position_ls = pos_ls;

	vec3 cam_vec = cam_pos - pos_w;
	vec3 n = normalize(vec3(0, 1, 0) + vec3(distortion.x, 0, distortion.y) * water_depth);
	float vr = max(0, dot(reflect(-normalize(light_direction), n), -normalize(cam_vec)));
	vec3 spec = vec3(0.4) * pow(vr, 10);

	float fresnell = pow(dot(normalize(cam_vec), n), 0.5);
	refr = mix(refr, vec4(0, 1, 1, 1), 0.1);
	vec4 refl_refr = mix(refl, refr, fresnell);

	//color = vec4(mix(refl_refr, water_tex, 0.25).rgb + spec, 0);
	color = vec4(refl_refr.rgb + spec, 1);
	color.a = water_depth;
}
