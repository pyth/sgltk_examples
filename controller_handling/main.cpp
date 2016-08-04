#include <sgltk/app.h>
#include <sgltk/window.h>

using namespace std;

class Win : public sgltk::Window {
public:
	Win(const char *title, int res_x, int res_y, int offset_x,
		int offset_y, int gl_maj, int gl_min, unsigned int flags);
	~Win();
	void handle_gamepad_added(unsigned int gamepad_id);
	void handle_gamepad_removed(unsigned int gamepad_id);
	void handle_gamepad_button(unsigned int gamepad_id, int button, bool pressed);
	void handle_gamepad_axis(unsigned int gamepad_id, unsigned int axis, int value);
	void handle_key_press(std::string key, bool pressed);
};

Win::Win(const char *title, int res_x, int res_y, int offset_x, int offset_y, int gl_maj, int gl_min, unsigned int flags) :
	sgltk::Window(title, res_x, res_y, offset_x, offset_y, gl_maj, gl_min, flags) {
}

Win::~Win() {}

void Win::handle_gamepad_added(unsigned int gamepad_id) {
}

void Win::handle_gamepad_removed(unsigned int gamepad_id) {
}

void Win::handle_gamepad_button(unsigned int gamepad_id, int button, bool pressed) {
	std::cout<<"player "<<gamepad_id<<": "<<"button "<<button<<" "<<pressed<<std::endl;
	sgltk::Gamepad *gamepad = sgltk::Gamepad::id_map[gamepad_id];
	gamepad->play_rumble(1, 200);
}

void Win::handle_gamepad_axis(unsigned int gamepad_id, unsigned int axis, int value) {
	std::cout<<"player "<<gamepad_id<<": "<<"axis "<<axis<<" "<<value<<std::endl;
}

void Win::handle_key_press(std::string key, bool pressed) {
	if(key == "Escape")
		stop();
}

int main(int argc, char **argv) {
	sgltk::App::init();
	SDL_Rect rect = sgltk::App::sys_info.display_bounds[0];
	Win window("Controller test", 600, 400, 100 + rect.x, 100 + rect.y, 3, 0, 0);
	window.run();
	return 0;
}
