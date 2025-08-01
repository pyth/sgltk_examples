#include <sgltk/sgltk.h>

using namespace sgltk;

class Win : public Window {
	bool shift;
	double scale;
	int interations;
	glm::dvec2 center;

	Texture_1d texture;
	Mesh display_mesh;
	Shader shader;
	P_Camera cam;

	void handle_resize();
	void handle_mouse_wheel(int x, int y);
	void handle_keyboard(const std::string& key);
	void handle_key_press(const std::string& key, bool pressed);
	void display();
public:
	Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y);
	~Win();
};

Win::Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y) :
		Window(title, res_x, res_y, offset_x, offset_y) {

	scale = 2.0;
	shift = false;
	interations = 100;
	center = glm::dvec2(0.5, 0);

	std::vector<glm::vec4> pos = {	glm::vec4(-1,  1, 0, 1),
					glm::vec4(-1, -1, 0, 1),
					glm::vec4( 1,  1, 0, 1),
					glm::vec4( 1, -1, 0, 1)};

	std::vector<glm::vec2> tc = {glm::vec2(0, 0),
				     glm::vec2(0, 1),
				     glm::vec2(1, 0),
				     glm::vec2(1, 1)};

	std::vector<unsigned short> ind = {0, 1, 2, 3};

	texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	texture.load("mandelbrot.png");

	shader.attach_file("mandelbrot_vs.glsl", GL_VERTEX_SHADER);
	shader.attach_file("mandelbrot_fs.glsl", GL_FRAGMENT_SHADER);
	shader.link();

	cam = P_Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1),
			glm::vec3(0, 1, 0), glm::radians(70.0f),
			(float)width, (float)height, 0.1f, 100.0f);

	display_mesh.setup_camera(&cam);
	display_mesh.setup_shader(&shader);
	display_mesh.attach_texture("texture_ambient", texture);
	display_mesh.add_vertex_attribute("pos_in", 4, GL_FLOAT, pos);
	display_mesh.add_vertex_attribute("tc_in", 2, GL_FLOAT, tc);
	display_mesh.attach_index_buffer(ind);
}

Win::~Win() {
}

void Win::display() {
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.set_uniform("screen_res", glm::vec2(width, height));
	shader.set_uniform("iter", interations);
	shader.set_uniform("scale", scale);
	shader.set_uniform("center", center);
	display_mesh.draw(GL_TRIANGLE_STRIP);
}

void Win::handle_resize() {
	glViewport(0, 0, width, height);
	cam.width = static_cast<float>(width);
	cam.height = static_cast<float>(height);
	cam.update_projection_matrix();
}

void Win::handle_key_press(const std::string& key, bool pressed) {
	if(key == "Left Shift" || key == "Right Shift")
		shift = pressed;
	else if(key == "Escape")
		stop();
}

void Win::handle_mouse_wheel(int x, int y) {
	scale -= 0.01 * y * scale;
}

void Win::handle_keyboard(const std::string& key) {
	double mov_speed = 50.0;

	if(key == "D") {
		center -= glm::dvec2(mov_speed * delta_time, 0) * scale;
	} else if(key == "A") {
		center += glm::dvec2(mov_speed * delta_time, 0) * scale;
	} else if(key == "W") {
		center += glm::dvec2(0, mov_speed * delta_time) * scale;
	} else if(key == "S") {
		center -= glm::dvec2(0, mov_speed * delta_time) * scale;
	} else if(key == "=" && shift) {
		scale -= 0.01 * scale;
	} else if(key == "-") {
		scale += 0.01 * scale;
	} else if(key == "Keypad +") {
		scale -= 0.01 * scale;
	} else if(key == "Keypad -") {
		scale += 0.01 * scale;
	}
}

int main(int argc, char **argv) {
	/*
	initialize the library
	this should be done prior to using any of the classes and
	functions provided by sgltk
	*/
	App::init();
	//change the current working directory to the location of the executable
	App::chdir_to_bin(argv);

	//setup the shader files location
	Shader::add_path("../mandelbrot_set/shaders");
	Texture::add_path("../data/textures");

	int w = (int)(0.75 * App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * App::sys_info.display_bounds[0].h);
	int x = App::sys_info.display_bounds[0].x +
		(int)(0.125 * App::sys_info.display_bounds[0].w);
	int y = App::sys_info.display_bounds[0].y +
		(int)(0.125 * App::sys_info.display_bounds[0].h);

	Win window("Mandelbrot set", w, h, x, y);
	if(window.gl_maj < 4)
		return -1;

	window.run();

	return 0;
}
