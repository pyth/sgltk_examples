#version 130

in vec3 position_in;
in vec3 velocity_in;
in vec2 lifetime_in;

out float life;

uniform float time;
uniform mat4 model_view_proj_matrix;

void main() {
	float t = time - lifetime_in.x;
	life = (lifetime_in.x + lifetime_in.y - time) / lifetime_in.y;
	vec3 pos = position_in + velocity_in * t;
	gl_PointSize = 10 * (life);
	gl_Position = model_view_proj_matrix * vec4(pos, 1);
}
