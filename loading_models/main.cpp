#include <sgltk/app.h>
#include <sgltk/scene.h>
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

static bool rel_mode;
static bool mouse_mode_change;

static Timer timer;
static Scene box;
static Scene bob;
static Scene spikey;
static Shader *box_shader;
static Shader *bob_shader;
static Shader *spikey_shader;
static Camera *camera;

class Win : public sgltk::Window {
public:
	Win(const char *title, int res_x, int res_y, int offset_x,
		int offset_y, int gl_maj, int gl_min, unsigned int flags);
	~Win();
	void handle_keyboard();
	void handle_mouse_motion(int x, int y);
	void display();
};

Win::Win(const char *title, int res_x, int res_y, int offset_x, int offset_y, int gl_maj, int gl_min, unsigned int flags) :
	sgltk::Window(title, res_x, res_y, offset_x, offset_y, gl_maj, gl_min, flags) {
}

Win::~Win() {}

void Win::display() {
	//clear the screen
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glm::vec3 light_pos(5, 10, 0);

	bob_shader->bind();
	int light_loc = glGetUniformLocation(bob_shader->program,
		"light_pos");
	glUniform3fv(light_loc, 1, glm::value_ptr(light_pos));

	int cam_loc = glGetUniformLocation(bob_shader->program,
		"cam_pos");
	glUniform3fv(cam_loc, 1, glm::value_ptr(camera->pos));

	spikey_shader->bind();
	light_loc = glGetUniformLocation(spikey_shader->program,
		"light_pos");
	glUniform3fv(light_loc, 1, glm::value_ptr(light_pos));

	cam_loc = glGetUniformLocation(spikey_shader->program,
		"cam_pos");
	glUniform3fv(cam_loc, 1, glm::value_ptr(camera->pos));

	box_shader->bind();
	light_loc = glGetUniformLocation(box_shader->program,
		"light_pos");
	glUniform3fv(light_loc, 1, glm::value_ptr(light_pos));

	cam_loc = glGetUniformLocation(box_shader->program,
		"cam_pos");
	glUniform3fv(cam_loc, 1, glm::value_ptr(camera->pos));

	//draw the models
	bob.animate(timer.get_time());
	bob.draw();

	glm::mat4 mat = glm::translate(glm::vec3(4, 0, 0));

	spikey.animate(timer.get_time());
	spikey.draw(&mat);

	mat = glm::translate(glm::vec3(-4, 0, 0));
	box.draw(&mat);
}

void Win::handle_keyboard() {
	float mov_speed = 0.1;
	float rot_speed = 0.05;
	float dt = 1000 * delta_time;
	if (dt < 2.0)
		dt = 2.0;

	if (key_pressed("Escape"))
		exit(0);
	if (key_pressed("D")) {
		camera->move_right(mov_speed * dt);
	}
	if (key_pressed("A")) {
		camera->move_right(-mov_speed * dt);
	}
	if (key_pressed("W")) {
		camera->move_forward(mov_speed * dt);
	}
	if (key_pressed("S")) {
		camera->move_forward(-mov_speed * dt);
	}
	if (key_pressed("R")) {
		camera->move_up(mov_speed * dt);
	}
	if (key_pressed("F")) {
		camera->move_up(-mov_speed * dt);
	}
	if (key_pressed("E")) {
		camera->roll(rot_speed * dt);
	}
	if (key_pressed("Q")) {
		camera->roll(-rot_speed * dt);
	}
	if (key_pressed("M") && !mouse_mode_change) {
		mouse_mode_change = true;
		rel_mode = !rel_mode;
		set_relative_mode(rel_mode);
	}
	else if (!key_pressed("M")) {
		mouse_mode_change = false;
	}
	camera->update_view_matrix();
}

void Win::handle_mouse_motion(int x, int y) {
	if (rel_mode) {
		camera->yaw(-glm::atan((float)x) / 500);
		camera->pitch(-glm::atan((float)y) / 500);
		camera->update_view_matrix();
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

	rel_mode = true;
	mouse_mode_change = false;

	//initialize the library
	App::init();

	//set the pathes to search for assets and shaders
	Scene::add_path("../data/models");
	Image::add_path("../data/textures");
	Shader::add_path("../loading_models/shaders");

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//open a window
	Win window("Loading models", 1024, 768, 100, 100, 3, 0, 0);
	window.set_relative_mode(true);

	//create shaders
	bob_shader = new Shader();
	bob_shader->attach_file("bob_vs.glsl", GL_VERTEX_SHADER);
	bob_shader->attach_file("bob_fs.glsl", GL_FRAGMENT_SHADER);
	bob_shader->link();

	spikey_shader = new Shader();
	spikey_shader->attach_file("spikey_vs.glsl", GL_VERTEX_SHADER);
	spikey_shader->attach_file("spikey_fs.glsl", GL_FRAGMENT_SHADER);
	spikey_shader->link();

	box_shader = new Shader();
	box_shader->attach_file("box_vs.glsl", GL_VERTEX_SHADER);
	box_shader->attach_file("box_fs.glsl", GL_FRAGMENT_SHADER);
	box_shader->link();

	//create a camera
	camera = new Camera(glm::vec3(0,5,20), glm::vec3(0,0,-1),
				glm::vec3(0,1,0), 70.0f,
				window.width, window.height,
				0.1f, 800.0f);

	//load the models and prepare them for rendering
	bob.setup_shader(bob_shader);
	bob.setup_camera(&camera->view_matrix, &camera->projection_matrix_persp);
	bob.load("bob_lamp.md5mesh");

	spikey.setup_shader(spikey_shader);
	spikey.setup_camera(&camera->view_matrix, &camera->projection_matrix_persp);
	spikey.load("Spikey.dae");

	box.setup_shader(box_shader);
	box.setup_camera(&camera->view_matrix, &camera->projection_matrix_persp);
	box.load("box.obj");

	//start the timer
	timer.start();

	//start the mainloop
	window.run();
	return 0;
}