#include <sgltk/app.h>
#include <sgltk/window.h>
#include <sgltk/scene.h>
#include <sgltk/camera.h>
#include <sgltk/shader.h>

#ifdef __linux__
	#include <unistd.h>
#else
	#include <direct.h>
#endif //__linux__


#define RES_X 1024
#define RES_Y 754


class Win : public sgltk::Window {
	bool rel_mode;

	sgltk::Scene *box;
	sgltk::Shader *shader;
	sgltk::Camera *cam;
public:
	Win(const char *title, int res_x, int res_y, int offset_x,
		int offset_y, int gl_maj, int gl_min, unsigned int flags);
	~Win();
	void handle_mouse_motion(int x, int y);
	void handle_key_press(std::string key, bool pressed);
	void handle_keyboard(std::string key);
	void display();
};

Win::Win(const char *title, int res_x, int res_y, int offset_x, int offset_y, int gl_maj, int gl_min, unsigned int flags) :
	sgltk::Window(title, res_x, res_y, offset_x, offset_y, gl_maj, gl_min, flags) {

	rel_mode = true;
	set_relative_mode(true);

	//compile and link the shaders
	shader = new sgltk::Shader();
	shader->attach_file("box_vs.glsl", GL_VERTEX_SHADER);
	shader->attach_file("box_fs.glsl", GL_FRAGMENT_SHADER);
	shader->link();

	cam = new sgltk::Camera(glm::vec3(25, 25, 100), glm::vec3(0, 0, -1),
				glm::vec3(0, 1, 0),
				70.0f, RES_X, RES_Y, 0.1f, 800.0f);

	std::vector<glm::mat4> model_matrix(25);
	for(unsigned int i = 0; i < model_matrix.size(); i++) {
		model_matrix[i] = glm::scale(glm::vec3(4));
		model_matrix[i] *= glm::translate(glm::vec3(i / 5 * 4, i % 5 * 4, 0));
	}

	//load a model
	box = new sgltk::Scene();
	box->setup_shader(shader);
	box->setup_camera(&cam->view_matrix, &cam->projection_matrix_persp);
	box->load("box.obj");
	box->setup_instanced_matrix(&model_matrix);
}

Win::~Win() {
	delete box;
	delete cam;
	delete shader;
}

void Win::handle_mouse_motion(int x, int y) {
	if (rel_mode) {
		cam->yaw(-glm::atan((float)x) / 500);
		cam->pitch(-glm::atan((float)y) / 500);
		cam->update_view_matrix();
	}
}

void Win::handle_key_press(std::string key, bool pressed) {
	if(key == "Escape") {
		stop();
	} else if(key == "M") {
		if(pressed) {
			rel_mode = !rel_mode;
			set_relative_mode(rel_mode);
		}
	}
}

void Win::handle_keyboard(std::string key) {
	float mov_speed = 0.1f;
	float rot_speed = 0.001f;
	float dt = 10 * (float)delta_time;
	if (dt < 1.0)
		dt = 1.0;

	if(key == "D") {
		cam->move_right(mov_speed * dt);
	} else if(key == "A") {
		cam->move_right(-mov_speed * dt);
	} else if(key == "W") {
		cam->move_forward(mov_speed * dt);
	} else if(key == "S") {
		cam->move_forward(-mov_speed * dt);
	} else if(key == "R") {
		cam->move_up(mov_speed * dt);
	} else if(key == "F") {
		cam->move_up(-mov_speed * dt);
	} else if(key == "E") {
		cam->roll(rot_speed * dt);
	} else if(key == "Q") {
		cam->roll(-rot_speed * dt);
	}
	cam->update_view_matrix();
}

void Win::display() {
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	shader->bind();
	int light_loc = glGetUniformLocation(shader->program,
		"light_pos");
	glUniform3f(light_loc, 25, 25, 20);
	int cam_loc = glGetUniformLocation(shader->program,
		"cam_pos");
	glUniform3fv(cam_loc, 1, glm::value_ptr(cam->pos));

	box->draw_instanced(25);
	//box->draw();
}

int main(int argc, char **argv) {
	//change the current working directory to the location
	//of the executable
	std::string path(argv[0]);
	path = path.substr(0, path.find_last_of("\\/"));
	#ifdef __linux__
		chdir(path.c_str());
	#else
		_chdir(path.c_str());
	#endif //__linux__

	//initialize the library
	//this should be done prior to using any of the classes and
	//functions provided by sgltk
	sgltk::App::init();

	//setup the shader files location
	sgltk::Shader::add_path("../instanced_rendering2/shaders");
	sgltk::Scene::add_path("../data/models");
	sgltk::Image::add_path("../data/textures");

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Win window("Instanced rendering 2",
					RES_X, RES_Y,
					100, 100,
					3, 3, 0);

	window.run();

	return 0;
}