#include <sgltk/app.h>

#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	sgltk::App::init();
	cout<<"System info"<<endl<<endl;
	cout<<"OS: "<<sgltk::App::sys_info.platform_name<<endl;
	cout<<"CPU Cores: "<<sgltk::App::sys_info.num_logical_cores<<endl;
	cout<<"RAM: "<<sgltk::App::sys_info.system_ram<<"MB"<<endl;
	cout<<"Number of displays: "<<sgltk::App::sys_info.num_displays<<endl;

	SDL_Rect bounds;
	SDL_DisplayMode mode;
	for(int i = 0; i < sgltk::App::sys_info.num_displays; i++) {
		mode = sgltk::App::sys_info.desktop_display_modes[i];
		bounds = sgltk::App::sys_info.display_bounds[i];
		cout<<"Display "<<i<<": "<<mode.w<<"x"<<mode.h<<"+"<<bounds.x<<
			"+"<<bounds.y<<" @"<<mode.refresh_rate<<endl;
	}

	cout<<"Supported OpenGL version: "<<
		sgltk::App::sys_info.gl_version_major<<"."<<
		sgltk::App::sys_info.gl_version_minor<<endl<<endl;

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
	return 0;
}
