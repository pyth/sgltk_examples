#version 330

in vec2 tc;

out vec4 color;

uniform sampler2D position_texture;
uniform sampler2D color_texture;
uniform sampler2D normal_texture;
uniform vec3 light_direction;
uniform vec3 cam_pos;

void main() {
	vec3 pos_w = texture(position_texture, vec2(tc.x, 1.0 - tc.y)).xyz;
	vec4 col = texture(color_texture, vec2(tc.x, 1.0 - tc.y));

	vec4 amb = vec4(0.2 * col.xyz, 1);
	vec3 l = normalize(light_direction);
	vec3 v = normalize(cam_pos - pos_w);
	vec3 n = texture(normal_texture, vec2(tc.x, 1.0 - tc.y)).xyz;
	float ln = max(0, dot(-l, n));
	vec4 diff = vec4(col.xyz, 1) * ln;
	float vr = max(0, dot(reflect(l, n), v));
	vec4 spec = texture(color_texture, vec2(tc.x, 1.0 - tc.y)).a * vec4(0.3) * pow(vr, 10);
	color = amb + diff + spec;
}
