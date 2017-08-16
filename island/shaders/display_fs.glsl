#version 330

in vec2 tc;

out vec4 color;

uniform sampler2D shadow_texture;
uniform sampler2D depth_texture;
uniform sampler2D position_texture;
uniform sampler2D position_ls_texture;
uniform sampler2D color_texture;
uniform sampler2D spec_texture;
uniform sampler2D normal_texture;
uniform vec3 light_direction;
uniform vec3 cam_pos;

float lin_depth(in float depth, in float near, in float far) {
	float d = 2.0 * depth - 1.0;
	return (2.0 * near * far) / (far + near - d * (far - near)) / far;
}

void main() {
	vec3 pos_w = texture(position_texture, vec2(tc.x, 1.0 - tc.y)).xyz;
	vec4 pos_ls = texture(position_ls_texture, vec2(tc.x, 1.0 - tc.y));
	vec4 col = texture(color_texture, vec2(tc.x, 1.0 - tc.y));
	float sp = texture(spec_texture, vec2(tc.x, 1.0 - tc.y)).r;

	vec3 pos_shadow = pos_ls.xyz / pos_ls.w * 0.5 + 0.5;
	float saved_depth = texture(shadow_texture, vec2(pos_shadow.x, pos_shadow.y)).r;

	vec4 amb = vec4(0.2 * col.xyz, 1);
	vec3 l = normalize(light_direction);
	vec3 v = normalize(cam_pos - pos_w);
	vec3 n = normalize(texture(normal_texture, vec2(tc.x, 1.0 - tc.y)).xyz);
	float ln = max(0, dot(-l, n));
	vec4 diff = vec4(col.xyz * ln, 1);
	float vr = max(0, dot(reflect(l, n), v));
	vec4 spec = sp * 0.1 * vec4(1) * pow(vr, 2);
	color = amb + diff + spec;
	//color = vec4(col.xyz, 1);
	//if(pos_shadow.z - 0.0005 > saved_depth)
	//	color = vec4(1, 0, 0, 1);
	//color = vec4(texture(shadow_texture, vec2(tc.x, 1.0 - tc.y)).r);
	//color = vec4(vec3(saved_depth), 1);
	//color = vec4(texture(shadow_texture, vec2(pos_shadow.x, pos_shadow.y)).r);
	//color = vec4(pos_shadow.z - saved_depth);
	//color = vec4(pos_shadow.z);
	//color = vec4(vec3(texture(shadow_texture, vec2(tc.x, 1.0 - tc.y)).r), 1);
}
