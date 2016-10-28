#include <sgltk/app.h>
#include <sgltk/window.h>
#include <sgltk/mesh.h>
#include <sgltk/scene.h>
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

	glm::vec2 cam_pos;

	glm::mat4 skybox_mat;
	glm::mat4 obj_mat;
	sgltk::Texture cubemap;
	sgltk::Mesh skybox;
	sgltk::Shader skybox_shader;
	sgltk::Shader obj_shader;
	sgltk::IP_Camera cam;
	sgltk::Scene obj;
public:
	Win(const std::string& title, int res_x, int res_y, int offset_x,
		int offset_y, int gl_maj, int gl_min, unsigned int flags);
	~Win();
	void handle_resize();
	void handle_keyboard(std::string key);
	void handle_key_press(std::string key, bool pressed);
	void handle_mouse_motion(int x, int y);
	void display();
};

Win::Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y, int gl_maj, int gl_min, unsigned int flags) :
		sgltk::Window(title, res_x, res_y, offset_x, offset_y, gl_maj, gl_min, flags) {

	rel_mode = true;
	wireframe = false;

	//skybox vertex positions
	std::vector<glm::vec4> pos = {
		glm::vec4(-1, -1, -1, 1),
		glm::vec4( 1, -1, -1, 1),
		glm::vec4(-1,  1, -1, 1),
		glm::vec4( 1,  1, -1, 1),

		glm::vec4(-1, -1,  1, 1),
		glm::vec4( 1, -1,  1, 1),
		glm::vec4(-1,  1,  1, 1),
		glm::vec4( 1,  1,  1, 1)
	};

	//triangle topology
	std::vector<unsigned short> ind = {
		//front
		0, 1, 2, 2, 1, 3,
		//back
		4, 5, 6, 6, 5, 7,
		//left
		4, 0, 6, 6, 0, 2,
		//right
		1, 5, 7, 1, 7, 2,
		//bottom
		0, 1, 4, 1, 4, 5,
		//top
		2, 3, 6, 3, 6, 7
	};

	//compile and link the shaders
	skybox_shader.attach_file("skybox_vs.glsl", GL_VERTEX_SHADER);
	skybox_shader.attach_file("skybox_fs.glsl", GL_FRAGMENT_SHADER);
	skybox_shader.link();

	obj_shader.attach_file("obj_vs.glsl", GL_VERTEX_SHADER);
	obj_shader.attach_file("obj_fs.glsl", GL_FRAGMENT_SHADER);
	obj_shader.link();

	//laod skybox textures
	sgltk::Image pos_x("pond_r.jpg");
	sgltk::Image neg_x("pond_l.jpg");
	sgltk::Image pos_y("pond_u.jpg");
	sgltk::Image neg_y("pond_d.jpg");
	sgltk::Image pos_z("pond_b.jpg");
	sgltk::Image neg_z("pond_f.jpg");
	cubemap.set_target(GL_TEXTURE_CUBE_MAP);
	cubemap.load_cubemap(pos_x, neg_x, pos_y, neg_y, pos_z, neg_z);

	cam = sgltk::IP_Camera(glm::vec3(0, 0, 10), glm::vec3(0, 0, -1),
			       glm::vec3(0, 1, 0), glm::radians(70.f),
			       (float)width, (float)height, 0.1f);

	//create the triangle mesh
	skybox_mat = glm::scale(glm::vec3(200));
	int pos_buf = skybox.attach_vertex_buffer<glm::vec4>(pos);
	skybox.attach_index_buffer(ind);
	skybox.setup_shader(&skybox_shader);
	skybox.setup_camera(&cam.view_matrix, &cam.projection_matrix);
	skybox.set_vertex_attribute("pos_in", pos_buf, 4, GL_FLOAT, 0, 0);
	skybox.textures_ambient = {&cubemap};

	obj_mat = glm::scale(glm::vec3(2));
	obj.setup_camera(&cam.view_matrix, &cam.projection_matrix);
	obj.setup_shader(&obj_shader);
	obj.load("monkey.obj");
	obj.attach_texture("cubemap", &cubemap);

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
		cam.yaw(-glm::atan((float)x) * dt);
		cam.pitch(-glm::atan((float)y) * dt);
		cam.pos = -10.f * glm::normalize(cam.dir);
		cam.update_view_matrix();
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
	} else if(key == "L") {
		if(pressed) {
			wireframe = !wireframe;
		}
	}
}

void Win::handle_keyboard(std::string key) {
	float rot_speed = 0.005f;
	float dt = 1000 * (float)delta_time;
	if (dt < 2.0)
		dt = 2.0;

	if(key == "E") {
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	skybox.draw(GL_TRIANGLES, &skybox_mat);

	obj_shader.bind();
	obj_shader.set_uniform("cam_pos", cam.pos);
	obj.draw(&obj_mat);
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

	//setup asset locations
	sgltk::Shader::add_path("../cubemap/shaders");
	sgltk::Image::add_path("../data/textures");
	sgltk::Scene::add_path("../data/models");

	int w = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].h);

	Win window("Cubemap", w, h, x, y, 3, 0, 0);

	window.run(100);

	return 0;
}
