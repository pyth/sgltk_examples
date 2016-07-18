#include <sgltk/app.h>
#include <sgltk/window.h>

using namespace std;

int main(int argc, char **argv) {
	sgltk::App::init();
	SDL_Rect rect = sgltk::App::sys_info.display_bounds[0];
	sgltk::Window window("Controller test", 600, 400, 100 + rect.x, 100 + rect.y, 3, 0, 0);
	window.run();
	return 0;
}

void sgltk::Window::handle_gamepad_added(unsigned int gamepad_id) {
}

void sgltk::Window::handle_gamepad_removed(unsigned int gamepad_id) {
}

void sgltk::Window::handle_gamepad_button(unsigned int gamepad_id, int button, bool pressed) {
	std::cout<<"player "<<gamepad_id<<": "<<button<<" "<<pressed<<std::endl;
	sgltk::Gamepad *gamepad = sgltk::Gamepad::id_map[gamepad_id];
	gamepad->play_rumble(0.5, 200);
}

void sgltk::Window::handle_gamepad_axis(unsigned int gamepad_id, unsigned int axis, int value) {
	std::cout<<"player "<<gamepad_id<<": "<<axis<<" "<<value<<std::endl;
}

void sgltk::Window::handle_keyboard() {
	if(key_pressed("Escape"))
		exit(0);
}
