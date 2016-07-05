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
	App::init(4, 0);
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

	window = new GUI("Test", 1024, 768, 100, 100, 0);

	window->run();
	return 0;
}
