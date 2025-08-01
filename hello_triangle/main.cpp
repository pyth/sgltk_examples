#include <sgltk/sgltk.h>

class Win : public sgltk::Window {
	sgltk::Mesh mesh;
	sgltk::Shader shader1;
	sgltk::Shader shader2;
	sgltk::P_Camera cam1;
	sgltk::P_Camera cam2;

	void handle_resize();
	void handle_key_press(const std::string& key, bool pressed);
	void display();
public:
	Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y);
	~Win();
};

Win::Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y) :
		sgltk::Window(title, res_x, res_y, offset_x, offset_y) {

	//triangle vertex positions
	std::vector<glm::vec4> pos = {	glm::vec4(-5, -5, 0, 1),
					glm::vec4(5, -5, 0, 1),
					glm::vec4(0, 5, 0, 1)};
	//triangle vertex color values
	std::vector<glm::vec4> color = {glm::vec4(1, 0, 0, 1),
					glm::vec4(0, 1, 0, 1),
					glm::vec4(0, 0, 1, 1)};
	//triangle topology
	std::vector<unsigned short> ind = {0, 1, 2};

	//compile and link the shaders
	shader1.attach_file("vs1.glsl", GL_VERTEX_SHADER);
	shader1.attach_file("fs1.glsl", GL_FRAGMENT_SHADER);
	shader1.link();

	shader2.attach_file("vs2.glsl", GL_VERTEX_SHADER);
	shader2.attach_file("fs2.glsl", GL_FRAGMENT_SHADER);
	shader2.link();

	cam1 = sgltk::P_Camera(glm::vec3(0, 0, 20), glm::vec3(0, 0, -1),
			       glm::vec3(0, 1, 0), glm::radians(70.0f),
			       (float)width, (float)height, 0.1f, 800.0f);
	cam2 = sgltk::P_Camera(glm::vec3(0, 4, 20), glm::vec3(0, 0, -1),
			       glm::vec3(0, 1, 0), glm::radians(70.0f),
			       (float)width, (float)height, 0.1f, 800.0f);

	//create the triangle mesh
	int pos_loc = mesh.attach_vertex_buffer(pos);
	int color_loc = mesh.attach_vertex_buffer(color);
	mesh.attach_index_buffer(ind);
	mesh.setup_shader(&shader1);
	mesh.setup_camera(&cam1);
	mesh.set_vertex_attribute("pos_in", pos_loc, 4, GL_FLOAT, 0, 0);
	mesh.set_vertex_attribute("color_in", color_loc, 4, GL_FLOAT, 0, 0);
}

Win::~Win() {
}

void Win::handle_resize() {
	glViewport(0, 0, width, height);
	cam1.width = static_cast<float>(width);
	cam1.height = static_cast<float>(height);
	cam2.width = static_cast<float>(width);
	cam2.height = static_cast<float>(height);
	cam1.update_projection_matrix();
	cam2.update_projection_matrix();
}

void Win::handle_key_press(const std::string& key, bool pressed) {
	if(key == "Escape")
		stop();
	if(key == "1")
		mesh.setup_shader(&shader1);
	if(key == "2")
		mesh.setup_shader(&shader2);
	if(key == "3")
		mesh.setup_camera(&cam1);
	if(key == "4")
		mesh.setup_camera(&cam2);
}

void Win::display() {
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mesh.draw(GL_TRIANGLES);
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
	sgltk::Shader::add_path("../../hello_triangle/shaders");

	int w = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].h);

	Win window("Hello triangle", w, h, x, y);

	window.run();

	return 0;
}
