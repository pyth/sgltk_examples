#version 330

in float vs_in;
out float vs_out;

void main() {
	vs_out = sqrt(vs_in);
}
