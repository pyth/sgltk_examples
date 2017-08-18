#version 330

in vec3 tc;

layout (location = 0) out vec4 color;
layout (location = 1) out float shadow;
layout (location = 2) out vec4 position;
layout (location = 3) out vec3 normals;
layout (location = 4) out float spec;

uniform samplerCube sky_texture;

void main() {
	color = vec4(texture(sky_texture, tc).rgb, 1);
	shadow = 0.0;
	position = vec4(vec3(0), 0);
	normals = vec3(0);
	spec = 0.0;
}
