#include <sgltk/sgltk.h>

class Win : public sgltk::Window {
	sgltk::Mesh mesh;
	sgltk::Shader shader;
	sgltk::P_Camera cam;

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
	shader.attach_file("vertex_shader.glsl", GL_VERTEX_SHADER);
	shader.attach_file("fragment_shader.glsl", GL_FRAGMENT_SHADER);
	shader.link();

	cam = sgltk::P_Camera(glm::vec3(30, 30, 100), glm::vec3(0, 0, -1),
			      glm::vec3(0, 1, 0), glm::radians(70.0f),
			      (float)width, (float)height, 0.1f, 800.0f);

	std::vector<glm::mat4> model_matrix(25);
	for(unsigned int i = 0; i < model_matrix.size(); i++) {
		model_matrix[i] = glm::translate(glm::vec3(i / 5 * 15, i % 5 * 15, 0));
	}

	//create the triangle mesh
	int pos_buf = mesh.attach_vertex_buffer(pos);
	int color_buf = mesh.attach_vertex_buffer(color);
	int mat_buf = mesh.attach_vertex_buffer(model_matrix);
	mesh.attach_index_buffer(ind);
	mesh.setup_shader(&shader);
	mesh.setup_camera(&cam.view_matrix, &cam.projection_matrix);
	mesh.set_vertex_attribute("pos_in", pos_buf, 4, GL_FLOAT, 0, 0);
	mesh.set_vertex_attribute("color_in", color_buf, 4, GL_FLOAT, 0, 0);

	int trafo_loc = shader.get_attribute_location("mod_mat");
	for(int i = 0; i < 4; i++) {
		mesh.set_vertex_attribute(trafo_loc + i, mat_buf, 4, GL_FLOAT,
					  sizeof(glm::mat4),
					  (GLvoid *)(i * sizeof(glm::vec4)), 1);
	}

}

Win::~Win() {
}

void Win::handle_resize() {
	glViewport(0, 0, width, height);
	cam.width = static_cast<float>(width);
	cam.width = static_cast<float>(height);
	cam.update_projection_matrix();
}

void Win::handle_key_press(const std::string& key, bool pressed) {
	if(key == "Escape")
		stop();
}

void Win::display() {
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mesh.draw_instanced(GL_TRIANGLES, 25);
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
	sgltk::Shader::add_path("../instanced_rendering/shaders");

	int w = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].h);

	Win window("Instanced rendering", w, h, x, y);
	if(window.gl_maj == 3 && window.gl_min < 3)
		return -1;

	window.run();

	return 0;
}
