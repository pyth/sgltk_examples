#include <sgltk/sgltk.h>

using namespace sgltk;

int main(int argc, char **argv) {
	App::init();
	App::chdir_to_bin(argv);
	Shader::add_path("../compute_test/shaders");

	Window win("compute_test", 100, 100, 100, 100, SDL_WINDOW_HIDDEN);
	if(win.gl_maj < 4 || win.gl_min < 3)
		return -1;

	Buffer output;
	Buffer data_buffer;

	std::vector<float> data = {100.f, 9.f, 16.f, 243.14f};
	data_buffer.load(data, GL_STATIC_DRAW);
	output.load<float>(data.size(), NULL, GL_STATIC_READ);

	Shader comp_shader;
	comp_shader.attach_file("shader_cs.glsl", GL_COMPUTE_SHADER);
	comp_shader.link();

	data_buffer.bind(GL_SHADER_STORAGE_BUFFER, 0);
	output.bind(GL_SHADER_STORAGE_BUFFER, 1);
	comp_shader.bind();
	glDispatchCompute(data.size(), 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	float out_data[4];
	output.store(0, 4 * sizeof(float), &out_data);
	for(unsigned int i = 0; i < 4; i++)
		std::cout << "square root of " << data[i] << " is " << out_data[i] << std::endl;

#ifndef __linux__
	int stop;
	std::cin >> stop;
#endif //__linux__

	return 0;
}
