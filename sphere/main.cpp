#include <sgltk/app.h>
#include <sgltk/window.h>
#include <sgltk/mesh.h>
#include <sgltk/camera.h>
#include <sgltk/shader.h>

#ifdef __linux__
	#include <unistd.h>
#else
	#include <direct.h>
#endif //__linux__

class Win : public sgltk::Window {
	bool rel_mode;
	bool wireframe;

	int tess_level;
	glm::vec3 light_pos;

	sgltk::Shader shader;
	sgltk::Shader light_shader;
	sgltk::P_Camera cam;
	sgltk::Mesh light;
	sgltk::Mesh sphere;

	void handle_resize();
	void handle_mouse_wheel(int x, int y);
	void handle_mouse_motion(int x, int y);
	void handle_keyboard(const std::string& key);
	void handle_key_press(const std::string& key, bool pressed);
	void display();
public:
	Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y);
	~Win();
};

Win::Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y) :
		sgltk::Window(title, res_x, res_y, offset_x, offset_y) {

	rel_mode = false;
	wireframe = false;
	set_relative_mode(rel_mode);

	tess_level = 10;
	light_pos = glm::vec3(2, 2, 0);

	//compile and link the shaders
	shader.attach_file("vs.glsl", GL_VERTEX_SHADER);
	shader.attach_file("tc.glsl", GL_TESS_CONTROL_SHADER);
	shader.attach_file("te.glsl", GL_TESS_EVALUATION_SHADER);
	shader.attach_file("fs.glsl", GL_FRAGMENT_SHADER);
	shader.link();

	cam = sgltk::P_Camera(glm::vec3(0, 0, 4), glm::vec3(0, 0, -1),
			      glm::vec3(0, 1, 0), glm::radians(70.0f),
			      (float)width, (float)height, 0.1f, 800.0f);

	//octahedron
	std::vector<glm::vec3> pos = {	{ 0,  1,  0},
					{ 0,  0,  1},
					{ 1,  0,  0},
					{ 0,  0, -1},
					{-1,  0,  0},
					{ 0, -1,  0}};

	std::vector<unsigned short> ind = {	0, 1, 2,
						0, 2, 3,
						0, 3, 4,
						0, 4, 1,
						5, 2, 1,
						5, 3, 2,
						5, 4, 3,
						5, 1, 4};

	sphere.attach_vertex_buffer<glm::vec3>(pos);
	sphere.attach_index_buffer(ind);
	sphere.setup_camera(&cam.view_matrix, &cam.projection_matrix);
	sphere.setup_shader(&shader);
	sphere.set_vertex_attribute("pos_in", 0, 3, GL_FLOAT, 0, 0);

	light_shader.attach_string(	"#version 130\n"
					"in vec4 pos_in;"
					"uniform vec3 light_pos;"
					"uniform mat4 view_proj_matrix;"
					"void main() {"
					"	gl_Position = view_proj_matrix * vec4(light_pos, 1);"
					"	gl_PointSize = 10;"
					"	gl_FrontColor = vec4(1);"
					"}"
					, GL_VERTEX_SHADER);
	light_shader.link();

	std::vector<glm::vec4> light_vert = {glm::vec4(0, 0, 0, 0)};
	std::vector<unsigned short> light_ind = {0};
	light.attach_vertex_buffer<glm::vec4>(light_vert);
	light.set_vertex_attribute("pos_in", 0, 4, GL_FLOAT, 0, 0);
	light.attach_index_buffer(light_ind);
	//light.setup_camera(&cam.view_matrix, &cam.projection_matrix);
	light.setup_camera(&cam);
	light.setup_shader(&light_shader);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glFrontFace(GL_CCW);
}

Win::~Win() {
}

void Win::handle_resize() {
	glViewport(0, 0, width, height);
	cam.update_projection_matrix((float)width, (float)height);
}

void Win::handle_key_press(const std::string& key, bool pressed) {
	if(key == "Escape") {
		stop();
	} else if(key == "M") {
		if(pressed) {
			rel_mode = !rel_mode;
			set_relative_mode(rel_mode);
		}
	} else if(key == "L") {
		if(pressed) {
			wireframe = !wireframe;
		}
	} else if(key == "P") {
		if(pressed) {
			shader.recompile();
		}
	}
}

void Win::handle_keyboard(const std::string& key) {
	float mov_speed = 0.1f;
	float rot_speed = 0.01f;
	float dt = 1000 * (float)delta_time;
	if (dt < 0.01)
		dt = 0.01f;
	if(dt > 1.0)
		dt = 1.0f;

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

void Win::handle_mouse_motion(int x, int y) {
	if(rel_mode) {
		float dt = (float)delta_time;
		if(dt < 0.01)
			dt = 0.01f;
		if(dt > 2.0)
			dt = 2.0;
		cam.yaw(-glm::atan((float)x) / 500);
		cam.pitch(-glm::atan((float)y) / 500);
		cam.update_view_matrix();
	}
}

void Win::handle_mouse_wheel(int x, int y) {
	if(tess_level + y < 1)
		return;
	tess_level += y;
}

void Win::display() {
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	shader.bind();
	shader.set_uniform_float("tess_level", tess_level);
	shader.set_uniform("light_pos", light_pos);
	sphere.draw(GL_PATCHES);

	light_shader.bind();
	light_shader.set_uniform("light_pos", light_pos);
	light.draw(GL_POINTS);
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
	sgltk::App::set_gl_version(4, 0);
	sgltk::App::set_msaa_sample_number(8);
	sgltk::Shader::add_path("../sphere/shaders");

	int w = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].h);

	Win window("Tessellation sphere", w, h, x, y);

	window.run(60);

	return 0;
}
