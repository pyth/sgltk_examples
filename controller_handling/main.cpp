#include <sgltk/sgltk.h>

using namespace std;

class Win : public sgltk::Window {
	void handle_gamepad_added(unsigned int gamepad_id) {
		std::cout << "Gamepad #" << gamepad_id << ": connected" << std::endl;
	}

	void handle_gamepad_removed(unsigned int gamepad_id) {
		std::cout << "Gamepad #" << gamepad_id << ": removed" << std::endl;
	}

	void handle_gamepad_button(unsigned int gamepad_id, int button) {
		std::cout << "Gamepad #" << gamepad_id << ": button #" << button << " is being held down" << std::endl;
		sgltk::Gamepad *gamepad = sgltk::Gamepad::id_map[gamepad_id];
		gamepad->play_rumble(1, 200);
	}

	void handle_gamepad_button_press(unsigned int gamepad_id, int button, bool pressed) {
		sgltk::Gamepad *gamepad = sgltk::Gamepad::id_map[gamepad_id];
		if(pressed) {
			std::cout << "Gamepad #" << gamepad_id << ": button #" << button << " was pressed" << std::endl;
		} else {
			std::cout << "Gamepad #" << gamepad_id << ": button #" << button << " was released" << std::endl;
			gamepad->stop_rumble();
		}
	}

	void handle_gamepad_axis_change(unsigned int gamepad_id, unsigned int axis, int value) {
		std::cout << "Gamepad #" << gamepad_id << ": axis #" << axis << " was moved to " << value << std::endl;
	}

	void handle_joystick_added(unsigned int joystick_id) {
		std::cout << "Joystick #" << joystick_id << ": connected" << std::endl;
		sgltk::Joystick *joystick = sgltk::Joystick::id_map[joystick_id];
		joystick->set_deadzone(200);
		//On the X52 buttons 23 through 25 correspond to the three mode switch posissions
		size_t found = joystick->name.find("Saitek X52");
		if(found != std::string::npos) {
			joystick->mark_switch(23);
			joystick->mark_switch(24);
			joystick->mark_switch(25);
		}
	}

	void handle_joystick_removed(unsigned int joystick_id) {
		std::cout << "Joystick #" << joystick_id << ": removed" << std::endl;
	}

	void handle_joystick_button(unsigned int joystick_id, int button) {
		std::cout << "Joystick #" << joystick_id << ": button #" << button << " is being held down" << std::endl;
	}

	void handle_joystick_button_press(unsigned int joystick_id, int button, bool pressed) {
		if(pressed) {
			std::cout << "Joystick #" << joystick_id << ": button #" << button << " was pressed" << std::endl;
		} else {
			std::cout << "Joystick #" << joystick_id << ": button #" << button << " was released" << std::endl;
		}
	}

	void handle_joystick_axis_change(unsigned int joystick_id, unsigned int axis, int value) {
		std::cout << "Joystick #" << joystick_id << ": axis #" << axis << " was moved to " << value << std::endl;
	}

	void handle_joystick_hat_change(unsigned int joystick_id, unsigned int hat, unsigned int value) {
		std::cout << "Joystick #" << joystick_id << ": hat #" << hat << " was moved to " << value << std::endl;
	}

	void handle_key_press(const std::string& key, bool pressed) {
		if(key == "Escape")
			stop();
	}

public:
	Win(const char *title, int res_x, int res_y, int offset_x, int offset_y) :
		sgltk::Window(title, res_x, res_y, offset_x, offset_y) {
	}
	~Win() {}
};

int main(int argc, char **argv) {
	sgltk::App::init();

	int w = (int)(0.25 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.25 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.375 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.375 * sgltk::App::sys_info.display_bounds[0].h);

	Win window("Controller test", w, h, x, y);
	window.run(30);
	return 0;
}
