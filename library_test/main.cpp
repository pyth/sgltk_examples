#include "gui.h"

using namespace sgltk;

int main(int argc, char** argv) {
	App::init();
	//change the current working directory to the one containing the executable
	App::chdir_to_bin(argv);

	//set the paths to the resources
	Model::add_path("../data/models");
	Image::add_path("../data/textures");
	Image::add_path("../data/fonts");
	Shader::add_path("../library_test/shaders");

	int w = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].h);

	GUI window("Test", w, h, x, y);
	if(window.gl_maj < 4)
		return -1;

	window.run(100);
	return 0;
}
