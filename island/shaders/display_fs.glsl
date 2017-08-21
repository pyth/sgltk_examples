#version 330

in vec2 tc;

out vec4 color;

uniform sampler2D shadow_texture;
uniform sampler2D position_texture;
uniform sampler2D color_texture;
uniform sampler2D spec_texture;
uniform sampler2D normal_texture;
uniform vec3 light_direction;
uniform vec3 cam_pos;

void main() {
	vec4 pos_w = texture(position_texture, vec2(tc.x, 1.0 - tc.y));
	vec4 col = texture(color_texture, vec2(tc.x, 1.0 - tc.y));
	if(pos_w.w == 0.0) {
		color = col;
	} else {
		float shadow = texture(shadow_texture, vec2(tc.x, 1.0 - tc.y)).r;
		float sp = texture(spec_texture, vec2(tc.x, 1.0 - tc.y)).r;
		vec3 norm = normalize(texture(normal_texture, vec2(tc.x, 1.0 - tc.y)).xyz);
		float bias = max(0.01 * (1.0 - dot(norm, light_direction)), 0.005);
		vec4 amb = vec4(0.2 * col.rgb, col.a);
		vec3 l = normalize(light_direction);
		vec3 v = normalize(cam_pos - pos_w.xyz);
		float ln = max(0, dot(-l, norm));
		vec4 diff = vec4(col.rgb * ln, col.a);
		float vr = max(0, dot(reflect(l, norm), v));
		vec4 spec = 0.3 * sp * vec4(vec3(1), col.a) * pow(vr, 2);
		color = amb + shadow * (diff + spec);
		//color = vec4(shadow);
	}
}
