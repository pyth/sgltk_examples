#include <sgltk/app.h>
#include <sgltk/window.h>
#include <sgltk/scene.h>
#include <sgltk/camera.h>
#include <sgltk/texture.h>
#ifdef __linux__
	#include <unistd.h>
#else
	#include <direct.h>
#endif //__linux__
#include <string.h>

#include "gui.h"

using namespace sgltk;

static GUI *window;

int main(int argc, char** argv) {
	App::init();
	//change the current working directory to the one containing the executable
	std::string path(argv[0]);
	path = path.substr(0, path.find_last_of("\\/"));
#ifdef __linux__
	chdir(path.c_str());
#else
	_chdir(path.c_str());
#endif //__linux__

	//set the paths to the resources
	Scene::add_path("../data/models");
	Image::add_path("../data/textures");
	Image::add_path("../data/fonts");
	Shader::add_path("../library_test/shaders");

	int w = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].h);

	window = new GUI("Test", w, h, x, y, 4, 0, 0);

	window->run();
	return 0;
}
