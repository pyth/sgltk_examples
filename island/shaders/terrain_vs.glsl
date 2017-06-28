#version 400

in vec4 vert_pos_in;
in vec2 tile_pos_in;

out vec3 pos_v;
out vec2 tile_pos_tc;

uniform uint terrain_side;
uniform int tile_size;
uniform mat4 view_matrix;

mat4 translate(float x, float y, float z) {
	vec4 m0 = vec4(1, 0, 0, 0);
	vec4 m1 = vec4(0, 1, 0, 0);
	vec4 m2 = vec4(0, 0, 1, 0);
	vec4 m3 = vec4(x, y, z, 1);
	return mat4(m0, m1, m2, m3);
}

mat4 scale(float x, float y, float z) {
	vec4 m0 = vec4(x, 0, 0, 0);
	vec4 m1 = vec4(0, y, 0, 0);
	vec4 m2 = vec4(0, 0, z, 0);
	vec4 m3 = vec4(0, 0, 0, 1);
	return mat4(m0, m1, m2, m3);
}

void main() {
	vec2 tile_coord = vec2(tile_size * tile_pos_in);
	mat4 model_matrix = translate(tile_size * (tile_pos_in.x - terrain_side / 2), 0,
		tile_size * (tile_pos_in.y - terrain_side / 2)) * scale(tile_size, 1, tile_size);
	vec4 pos = model_matrix * vert_pos_in;
	pos_v = (view_matrix * pos).xyz;
	tile_pos_tc = tile_pos_in;
	gl_Position = pos;
}
