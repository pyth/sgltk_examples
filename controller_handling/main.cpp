#include <sgltk/app.h>
#include <sgltk/window.h>

using namespace std;

//gamepad, player
static map<int, int> player_map;

int main(int argc, char **argv) {
	sgltk::App::init();
	SDL_Rect rect = sgltk::App::sys_info.display_bounds[0];
	sgltk::Window window("Controller test", 600, 400, 100 + rect.x, 100 + rect.y, 3, 0, 0);
	window.run();
	return 0;
}

void sgltk::Window::handle_gamepad_added(unsigned int gamepad_id) {
	bool found;
	int player_max = 0;
	int num_players = player_map.size();

	//find highest player number
	for(map<int, int>::iterator i = player_map.begin(); i != player_map.end(); i++) {
		if(i->second > player_max)
			player_max = i->second;
	}
	for(int i = 0; i < player_max; i++) {
		found = false;
		for(map<int, int>::iterator j = player_map.begin(); j != player_map.end(); j++) {
			if(j->second == i) {
				found = true;
				break;
			}
		}
		if(!found) {
			player_map[gamepad_id] = i;
			return;
		}
	}
	player_map[gamepad_id] = num_players;
}

void sgltk::Window::handle_gamepad_removed(unsigned int gamepad_id) {
	player_map.erase(gamepad_id);
}

void sgltk::Window::handle_gamepad_button(unsigned int gamepad_id, int button, bool pressed) {
	std::cout<<"player "<<player_map[gamepad_id]<<": "<<button<<" "<<pressed<<std::endl;
	play_rumble(gamepad_id, 0.5, 200);
}

void sgltk::Window::handle_gamepad_axis(unsigned int gamepad_id, unsigned int axis, int value) {
	std::cout<<"player "<<player_map[gamepad_id]<<": "<<axis<<" "<<value<<std::endl;
}

void sgltk::Window::handle_keyboard() {
	if(key_pressed("Escape"))
		exit(0);
}
