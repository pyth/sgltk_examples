#include <sgltk/app.h>
#include <sgltk/window.h>

using namespace std;

class Win : public sgltk::Window {
	void handle_gamepad_added(unsigned int gamepad_id);
	void handle_gamepad_removed(unsigned int gamepad_id);
	void handle_gamepad_button(unsigned int gamepad_id, int button);
	void handle_gamepad_button_press(unsigned int gamepad_id, int button, bool pressed);
	void handle_gamepad_axis_change(unsigned int gamepad_id, unsigned int axis, int value);
	void handle_key_press(std::string key, bool pressed);
public:
	Win(const char *title, int res_x, int res_y, int offset_x, int offset_y,
		int gl_maj, int gl_min, int depth_bits, int stencil_bits, unsigned int flags);
	~Win();
};

Win::Win(const char *title, int res_x, int res_y, int offset_x, int offset_y,
	int gl_maj, int gl_min, int depth_bits, int stencil_bits, unsigned int flags) :
	sgltk::Window(title, res_x, res_y, offset_x, offset_y, gl_maj, gl_min, depth_bits, stencil_bits, flags) {
}

Win::~Win() {}

void Win::handle_gamepad_added(unsigned int gamepad_id) {
}

void Win::handle_gamepad_removed(unsigned int gamepad_id) {
}

void Win::handle_gamepad_button(unsigned int gamepad_id, int button) {
	std::cout<<"player "<<gamepad_id<<": "<<"button "<<button<<std::endl;
	sgltk::Gamepad *gamepad = sgltk::Gamepad::id_map[gamepad_id];
	gamepad->play_rumble(1, 200);
}

void Win::handle_gamepad_button_press(unsigned int gamepad_id, int button, bool pressed) {
	sgltk::Gamepad *gamepad = sgltk::Gamepad::id_map[gamepad_id];
	if(!pressed)
		gamepad->stop_rumble();
}

void Win::handle_gamepad_axis_change(unsigned int gamepad_id, unsigned int axis, int value) {
	std::cout<<"player "<<gamepad_id<<": "<<"axis "<<axis<<" "<<value<<std::endl;
}

void Win::handle_key_press(std::string key, bool pressed) {
	if(key == "Escape")
		stop();
}

int main(int argc, char **argv) {
	sgltk::App::init();

	int w = (int)(0.25 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.25 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.375 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.375 * sgltk::App::sys_info.display_bounds[0].h);

	Win window("Controller test", w, h, x, y, 3, 0, 24, 8, 0);
	window.run();
	return 0;
}
