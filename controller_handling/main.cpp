#include <sgltk/sgltk.h>

using namespace std;

class Win : public sgltk::Window {
	void handle_gamepad_added(std::shared_ptr<sgltk::Gamepad> gamepad) {
		cout << "Gamepad #" << gamepad->id << ": connected" << endl;
	}

	void handle_gamepad_removed(unsigned int gamepad_id) {
		cout << "Gamepad #" << gamepad_id << ": removed" << endl;
	}

	void handle_gamepad_button(std::shared_ptr<sgltk::Gamepad> gamepad, int button) {
		cout << "Gamepad #" << gamepad->id << ": button #" << button << " is being held down" << endl;
		gamepad->play_rumble(1, 200);
	}

	void handle_gamepad_button_press(std::shared_ptr<sgltk::Gamepad> gamepad, int button, bool pressed) {
		if(pressed) {
			cout << "Gamepad #" << gamepad->id << ": button #" << button << " was pressed" << endl;
		} else {
			cout << "Gamepad #" << gamepad->id << ": button #" << button << " was released" << endl;
			gamepad->stop_rumble();
		}
	}

	void handle_gamepad_axis_change(std::shared_ptr<sgltk::Gamepad> gamepad, unsigned int axis, int value) {
		cout << "Gamepad #" << gamepad->id << ": axis #" << axis << " was moved to " << value << endl;
	}

	void handle_joystick_added(std::shared_ptr<sgltk::Joystick> joystick) {
		cout << "Joystick #" << joystick->id << ": connected" << endl;
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
		cout << "Joystick #" << joystick_id << ": removed" << endl;
	}

	void handle_joystick_button(std::shared_ptr<sgltk::Joystick> joystick, int button) {
		cout << "Joystick #" << joystick->id << ": button #" << button << " is being held down" << endl;
	}

	void handle_joystick_button_press(std::shared_ptr<sgltk::Joystick> joystick, int button, bool pressed) {
		if(pressed) {
			cout << "Joystick #" << joystick->id << ": button #" << button << " was pressed" << endl;
		} else {
			cout << "Joystick #" << joystick->id << ": button #" << button << " was released" << endl;
		}
	}

	void handle_joystick_axis_change(std::shared_ptr<sgltk::Joystick> joystick, unsigned int axis, int value) {
		cout << "Joystick #" << joystick->id << ": axis #" << axis << " was moved to " << value << endl;
	}

	void handle_joystick_hat_change(std::shared_ptr<sgltk::Joystick> joystick, unsigned int hat, unsigned int value) {
		cout << "Joystick #" << joystick->id << ": hat #" << hat << " was moved to " << value << endl;
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
