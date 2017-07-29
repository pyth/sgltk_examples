#include <sgltk/sgltk.h>

class Win : public sgltk::Window {
	bool rel_mode;

	sgltk::Model box;
	sgltk::Shader shader;
	sgltk::P_Camera cam;

	void handle_resize();
	void handle_mouse_motion(int x, int y);
	void handle_key_press(const std::string& key, bool pressed);
	void handle_keyboard(const std::string& key);
	void display();
public:
	Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y);
	~Win();
};

Win::Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y) :
	sgltk::Window(title, res_x, res_y, offset_x, offset_y) {


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	rel_mode = true;
	set_relative_mode(true);

	//compile and link the shaders
	shader.attach_file("box_vs.glsl", GL_VERTEX_SHADER);
	shader.attach_file("box_fs.glsl", GL_FRAGMENT_SHADER);
	shader.link();

	cam = sgltk::P_Camera(glm::vec3(35, 35, 100), glm::vec3(0, 0, -1),
			      glm::vec3(0, 1, 0), glm::radians(70.0f),
			      (float)width, (float)height, 0.1f, 800.0f);

	std::vector<glm::mat4> model_matrix(125);
	for(unsigned int i = 0; i < 25; i++) {
		for(unsigned int j = 0; j < 5; j++) {
			model_matrix[j + i * 5] = glm::scale(glm::vec3(4));
			model_matrix[j + i * 5] *= glm::translate(glm::vec3(i / 5 * 4, i % 5 * 4, j * 4));
		}
	}

	//load a model
	box.setup_shader(&shader);
	box.setup_camera(&cam.view_matrix, &cam.projection_matrix);
	box.load("box.obj");
	box.set_texture_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	box.set_texture_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	box.setup_instanced_matrix(model_matrix);
}

Win::~Win() {
}

void Win::handle_resize() {
	glViewport(0, 0, width, height);
	cam.update_projection_matrix((float)width, (float)height);
}

void Win::handle_mouse_motion(int x, int y) {
	float dt = (float)delta_time;
	if (dt < 0.01)
		dt = 0.01f;
	if (rel_mode) {
		cam.yaw(-glm::atan((float)x) * dt);
		cam.pitch(-glm::atan((float)y) * dt);
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

void Win::handle_keyboard(const std::string& key) {
	float mov_speed = 1;
	float rot_speed = 0.05f;
	float dt = 1000 * (float)delta_time;
	if (dt < 1.0)
		dt = 1.0;

	if(key == "D") {
		cam.move_right(mov_speed * dt);
	} else if(key == "A") {
		cam.move_right(-mov_speed * dt);
	} else if(key == "W") {
		cam.move_forward(mov_speed * dt);
	} else if(key == "S") {
		cam.move_forward(-mov_speed * dt);
	} else if(key == "R") {
		cam.move_up(mov_speed * dt);
	} else if(key == "F") {
		cam.move_up(-mov_speed * dt);
	} else if(key == "E") {
		cam.roll(rot_speed * dt);
	} else if(key == "Q") {
		cam.roll(-rot_speed * dt);
	}
	cam.update_view_matrix();
}

void Win::display() {
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.set_uniform_float("light_pos", 25, 25, 20);
	shader.set_uniform("cam_pos", cam.pos);

	box.draw_instanced(125);
}

int main(int argc, char **argv) {
	/*
	initialize the library
	this should be done prior to using any of the classes and
	functions provided by sgltk
	*/
	sgltk::App::init();
	//change the current working directory to the location of the executable
	sgltk::App::chdir_to_bin(argv);

	//setup the shader files location
	sgltk::Shader::add_path("../instanced_rendering2/shaders");
	sgltk::Model::add_path("../data/models");
	sgltk::Image::add_path("../data/textures");

	int w = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].h);

	Win window("Instanced rendering 2", w, h, x, y);
	if(window.gl_maj == 3 && window.gl_min < 3)
		return -1;

	window.run();

	return 0;
}
