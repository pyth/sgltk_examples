#version 130

in vec3 position_in;
in vec3 velocity_in;
in vec2 lifetime_in;

out float life;

uniform float time;
uniform mat4 model_view_proj_matrix;

void main() {
	life = ((lifetime_in.y - lifetime_in.x) - mod(time - lifetime_in.x, lifetime_in.y - lifetime_in.x)) / (lifetime_in.y - lifetime_in.x);
	gl_PointSize = 10 * life;

	vec3 pos = position_in + velocity_in * mod(time - lifetime_in.x, lifetime_in.y - lifetime_in.x);
	gl_Position = model_view_proj_matrix * vec4(pos, 1);
}
