#include <sgltk/sgltk.h>

#ifdef __linux__
	#include <unistd.h>
#else
	#include <direct.h>
#endif //__linux__

using namespace sgltk;

int main(int argc, char **argv) {
	std::string path(argv[0]);
	path = path.substr(0, path.find_last_of("\\/"));
#ifdef __linux__
	chdir(path.c_str());
#else
	_chdir(path.c_str());
#endif

	App::init();
	Shader::add_path("../transform_feedback/shaders");

	Window win("transform_feedback", 100, 100, 100, 100, SDL_WINDOW_HIDDEN);
	if(win.gl_maj < 3)
		return -1;

	std::vector<std::string> attributes = {"vs_out"};
	Shader tf_shader;
	tf_shader.attach_file("shader_vs.glsl", GL_VERTEX_SHADER);
	tf_shader.set_transform_feedback_variables(attributes, GL_SEPARATE_ATTRIBS);
	tf_shader.link();

	Buffer output_buffer;
	output_buffer.create_empty<float>(4, GL_STATIC_READ);

	Mesh mesh;
	std::vector<float> data = {100.f, 9.f, 16.f, 243.14f};
	std::vector<unsigned short> ind = {0, 1, 2, 3};
	mesh.setup_shader(&tf_shader);
	mesh.add_vertex_attribute("vs_in", 1, GL_FLOAT, data);
	mesh.attach_index_buffer(ind);
	mesh.attach_buffer(&output_buffer, GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	mesh.draw(GL_POINTS);

	float out_data[4];
	output_buffer.store(0, 4 * sizeof(float), &out_data);
	for(unsigned int i = 0; i < 4; i++)
		std::cout << "square root of " << data[i] << " is " << out_data[i] << std::endl;

#ifndef __linux__
	int stop;
	std::cin >> stop;
#endif //__linux__

	return 0;
}
