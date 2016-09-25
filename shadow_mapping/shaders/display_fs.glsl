#version 130

in vec4 pos_eye;
in vec3 tc;

out vec4 color;

uniform sampler2D textures_diffuse;

void main() {
	color = texture(textures_diffuse, tc.xy);
}
