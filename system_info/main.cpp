#include <sgltk/sgltk.h>

using namespace std;

int main(int argc, char **argv) {
	sgltk::App::init();
	sgltk::Window win("System info", 1200, 800, 100, 100, SDL_WINDOW_HIDDEN);
	cout<<"System info"<<endl<<endl;
	cout<<"OS: "<<sgltk::App::sys_info.platform_name<<endl;
	cout<<"CPU Cores: "<<sgltk::App::sys_info.num_logical_cores<<endl;
	cout<<"RAM: "<<sgltk::App::sys_info.system_ram<<"MB"<<endl;
	cout<<"Highest supported OpenGL version: "<<win.gl_maj<<"."<<win.gl_min<<std::endl;
	cout<<"Number of displays: "<<sgltk::App::sys_info.num_displays<<endl;

	SDL_Rect bounds;
	SDL_DisplayMode mode;
	for(int i = 0; i < sgltk::App::sys_info.num_displays; i++) {
		mode = sgltk::App::sys_info.desktop_display_modes[i];
		bounds = sgltk::App::sys_info.display_bounds[i];
		cout<<"Display "<<i<<": "<<mode.w<<"x"<<mode.h<<"+"<<bounds.x<<
			"+"<<bounds.y<<" @"<<mode.refresh_rate<<endl;
	}

	int num_modes;
	for(int i = 0; i < sgltk::App::sys_info.num_displays; i++) {
		cout<<"Display "<<i<<" supports:"<<endl;
		num_modes = sgltk::App::sys_info.supported_display_modes[i].size();
		for(int j = 0; j < num_modes; j++){
			mode = sgltk::App::sys_info.supported_display_modes[i][j];
			cout<<mode.w<<"x"<<mode.h<<" @"<<mode.refresh_rate<<"Hz"<<endl;
		}
		cout<<endl;
	}
#ifdef _WIN32
	int foo;
	std::cin >> foo;
#endif
	return 0;
}
