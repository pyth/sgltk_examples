#include "sgltk/app.h"
#include "sgltk/window.h"
#include "sgltk/shader.h"
#include "sgltk/camera.h"
#include "sgltk/particle.h"

#include <stdlib.h>
#include <time.h>

#ifdef __linux__
	#include <unistd.h>
#else
	#include <direct.h>
#endif //__linux__

class Win : public sgltk::Window {
	bool rel_mode;
	unsigned int fps;
	unsigned int frame_cnt;
	unsigned int frame_sum;
	sgltk::P_Camera cam;
	sgltk::Shader shader;
	unsigned int num_particles;
	sgltk::Particles particle_system;

	void handle_resize();
	void handle_mouse_motion(int x, int y);
	void handle_key_press(const std::string& key, bool pressed);
	void display();
public:
	Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y);
	~Win();
};

Win::Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y) :
	 sgltk::Window(title, res_x, res_y, offset_x, offset_y) {

	rel_mode = true;
	fps = 0;
	frame_sum = 0;
	frame_cnt = 0;
	srand((unsigned int)time(NULL));

	num_particles = 300000;

	shader.attach_file("particle_vs.glsl", GL_VERTEX_SHADER);
	shader.attach_file("particle_fs.glsl", GL_FRAGMENT_SHADER);
	shader.link();

	cam = sgltk::P_Camera(glm::vec3(0, 0, 10), glm::vec3(0, 0, -1),
			      glm::vec3(0, 1, 0), glm::radians(70.f),
			      (float)width, (float)height, 0.1f, 800.0f);

	particle_system.resize(num_particles);
	particle_system.setup_camera(&cam.view_matrix, &cam.projection_matrix);
	particle_system.setup_shader(&shader);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
	set_relative_mode(rel_mode);
}

Win::~Win() {
}

void Win::handle_resize() {
	glViewport(0, 0, width, height);
	cam.update_projection_matrix((float)width, (float)height);
}

void Win::handle_mouse_motion(int x, int y) {
	if(rel_mode) {
		float dt = (float)delta_time;
		if (dt < 0.01)
			dt = 0.01f;
		if (dt > 3.0)
			dt = 3.0f;
		cam.yaw(-glm::atan((float)x) * dt);
		cam.pitch(-glm::atan((float)y) * dt);
		cam.pos = -10.f * glm::normalize(cam.dir);
		cam.update_view_matrix();
	}
}

void Win::handle_key_press(const std::string& key, bool pressed) {
	if(key == "Escape") {
		stop();
	} else if(key == "M") {
		if(pressed) {
			rel_mode = !rel_mode;
			set_relative_mode(rel_mode);
		}
	}
}

void Win::display() {
	glm::vec3 pos;
	glm::vec3 vel;
	float lt;


	pos = glm::vec3(0);
	vel = glm::vec3(2 * (float)rand() / RAND_MAX - 1, 2 * (float)rand() / RAND_MAX - 1, 2 * (float)rand() / RAND_MAX - 1);
	vel = (3 * (float)rand() / RAND_MAX + 0.5f) * glm::normalize(vel);
	lt = (float)(rand() % 30 + 5);
	particle_system.add_particle(pos, vel, lt);
	particle_system.update();

	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	particle_system.draw();
	frame_cnt++;
	frame_sum += (unsigned int)(1.0 / delta_time);
	if(frame_cnt > 100) {
		fps = (unsigned int)(frame_sum / frame_cnt);
		frame_cnt = 0;
		frame_sum = 0;
	}
	set_title("Particle system "+std::to_string(fps));
}

int main(int argc, char **argv) {
	std::string path(argv[0]);
	path = path.substr(0, path.find_last_of("\\/"));
	#ifdef __linux__
		chdir(path.c_str());
	#else
		_chdir(path.c_str());
	#endif //__linux__

	sgltk::App::init();
	sgltk::Shader::add_path("../particle_system/shaders");

	int w = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].h);

	Win window("Particle system", w, h, x, y);

	window.run();

	return 0;
}
