#include <sgltk/app.h>

#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	sgltk::App::init();
	cout<<"System info"<<endl<<endl;
	cout<<"OS: "<<sgltk::App::sys_info.platform_name<<endl;
	cout<<"CPU Cores: "<<sgltk::App::sys_info.num_logical_cores<<endl;
	cout<<"RAM: "<<sgltk::App::sys_info.system_ram<<" MB"<<endl;
	cout<<"Highest supported OpenGL version: "<<
		sgltk::App::sys_info.gl_version_major<<"."<<
		sgltk::App::sys_info.gl_version_minor<<endl;
	return 0;
}
