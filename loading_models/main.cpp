#include <sgltk/app.h>
#include <sgltk/model.h>
#include <sgltk/camera.h>
#include <sgltk/timer.h>
#include <sgltk/window.h>

#ifdef __linux__
	#include <unistd.h>
#else
	#include <direct.h>
#endif //__linux__
#include <string.h>

using namespace sgltk;

class Win : public sgltk::Window {
	bool rel_mode;

	Timer timer;
	Model box;
	Model bob;
	Model spikey;
	Shader box_shader;
	Shader bob_shader;
	Shader spikey_shader;
	P_Camera camera;

	void handle_resize();
	void handle_keyboard(const std::string& key);
	void handle_key_press(const std::string& key, bool pressed);
	void handle_mouse_motion(int x, int y);
	void display();
public:
	Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y);
	~Win();
};

Win::Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y) :
	 sgltk::Window(title, res_x, res_y, offset_x, offset_y) {

	rel_mode = true;
	set_relative_mode(rel_mode);

	//create shaders
	bob_shader.attach_file("bob_vs.glsl", GL_VERTEX_SHADER);
	bob_shader.attach_file("bob_fs.glsl", GL_FRAGMENT_SHADER);
	bob_shader.link();

	spikey_shader.attach_file("spikey_vs.glsl", GL_VERTEX_SHADER);
	spikey_shader.attach_file("spikey_fs.glsl", GL_FRAGMENT_SHADER);
	spikey_shader.link();

	box_shader.attach_file("box_vs.glsl", GL_VERTEX_SHADER);
	box_shader.attach_file("box_fs.glsl", GL_FRAGMENT_SHADER);
	box_shader.link();

	//create a camera
	camera = P_Camera(glm::vec3(0,5,20), glm::vec3(0,0,-1),
			  glm::vec3(0,1,0), glm::radians(70.0f),
			  (float)width, (float)height, 0.1f, 800.0f);

	//load the models and prepare them for rendering
	bob.setup_shader(&bob_shader);
	bob.setup_camera(&camera.view_matrix, &camera.projection_matrix);
	bob.load("bob_lamp.md5mesh");

	spikey.setup_shader(&spikey_shader);
	spikey.setup_camera(&camera.view_matrix, &camera.projection_matrix);
	spikey.load("Spikey.dae");
	spikey.set_texture_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	box.setup_shader(&box_shader);
	box.setup_camera(&camera.view_matrix, &camera.projection_matrix);
	box.load("box.obj");
	box.set_texture_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//start the timer
	timer.start();

}

Win::~Win() {}

void Win::handle_resize() {
	glViewport(0, 0, width, height);
	camera.update_projection_matrix((float)width, (float)height);
}

void Win::display() {
	//clear the screen
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glm::vec3 light_pos(5, 10, 0);

	bob_shader.set_uniform("light_pos", light_pos);
	bob_shader.set_uniform("cam_pos", camera.pos);

	spikey_shader.set_uniform("light_pos", light_pos);
	spikey_shader.set_uniform("cam_pos", camera.pos);

	box_shader.set_uniform("light_pos", light_pos);
	box_shader.set_uniform("cam_pos", camera.pos);

	//draw the models
	bob.animate((float)timer.get_time());
	bob.draw();

	glm::mat4 mat = glm::translate(glm::vec3(4, 0, 0));

	spikey.animate((float)timer.get_time());
	spikey.draw(&mat);

	mat = glm::translate(glm::vec3(-4, 0, 0));
	box.draw(&mat);
}

void Win::handle_keyboard(const std::string& key) {
	float mov_speed = 0.1f;
	float rot_speed = 0.01f;
	float dt = 1000 * (float)delta_time;
	if (dt < 0.001)
		dt = 0.001f;
	if(dt > 1.0)
		dt = 1.0f;

	if(key == "D") {
		camera.move_right(mov_speed * dt);
	} else if(key == "A") {
		camera.move_right(-mov_speed * dt);
	} else if(key == "W") {
		camera.move_forward(mov_speed * dt);
	} else if(key == "S") {
		camera.move_forward(-mov_speed * dt);
	} else if(key == "R") {
		camera.move_up(mov_speed * dt);
	} else if(key == "F") {
		camera.move_up(-mov_speed * dt);
	} else if(key == "E") {
		camera.roll(rot_speed * dt);
	} else if(key == "Q") {
		camera.roll(-rot_speed * dt);
	}
	camera.update_view_matrix();
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

void Win::handle_mouse_motion(int x, int y) {
	if (rel_mode) {
		float dt = (float)delta_time;
		if(dt < 0.01)
			dt = 0.01f;
		if(dt > 2.0)
			dt = 2.0;
		camera.yaw(-glm::atan((float)x) / 500);
		camera.pitch(-glm::atan((float)y) / 500);
		camera.update_view_matrix();
	}
}

int main(int argc, char **argv) {
	//change the current working directory to the one containing the executable
	std::string path(argv[0]);
	path = path.substr(0, path.find_last_of("\\/"));
#ifdef __linux__
	chdir(path.c_str());
#else
	_chdir(path.c_str());
#endif //__linux__

	//initialize the library
	App::init();

	//set the pathes to search for assets and shaders
	Model::add_path("../data/models");
	Image::add_path("../data/textures");
	Shader::add_path("../loading_models/shaders");

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int w = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].h);

	//open a window
	Win window("Loading models", w, h, x, y);

	//start the mainloop
	window.run();
	return 0;
}
