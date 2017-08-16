#version 330

in vec3 tc;

layout (location = 0) out vec4 color;
/*layout (location = 1) out vec3 normals;
layout (location = 2) out vec3 position;
layout (location = 3) out vec4 position_ls;
layout (location = 4) out float spec;*/

uniform samplerCube sky_texture;

void main() {
	color = vec4(texture(sky_texture, tc).rgb, 1);
	/*position_ls = vec4();
	spec = 0.0f;*/
}
