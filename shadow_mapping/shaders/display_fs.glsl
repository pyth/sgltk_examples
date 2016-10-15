#version 130

in vec4 pos_eye;
in vec3 tc;

out vec4 color;

uniform int perspective;
uniform float near;
uniform float far;
uniform sampler2D textures_diffuse;

float linearize_depth(float depth) {
	float d = 2.0 * depth - 1.0;
	return (2.0 * near * far) / (far + near - d * (far - near)) / far;
}

void main() {
	float depth = texture(textures_diffuse, tc.xy).r;
	if(perspective == 1)
		depth = linearize_depth(depth);
	color = vec4(vec3(depth), 1);
}
