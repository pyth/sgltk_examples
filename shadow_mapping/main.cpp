#include <sgltk/scene.h>
#include <sgltk/camera.h>
#include <sgltk/timer.h>
#include <sgltk/framebuffer.h>
#include <sgltk/window.h>

#ifdef __linux__
	#include <unistd.h>
#else
	#include <direct.h>
#endif //__linux__
#include <string.h>

using namespace sgltk;

class Win : public sgltk::Window {
	Mesh floor;
	Mesh depth_display;
	Scene box;
	Shader display_shader;
	Shader box_shader;
	Shader floor_shader;
	Shader shadow_shader;
	Camera camera;
	Camera light_cam;
	Texture depth_tex;
	Framebuffer frame_buf;

	glm::mat4 light_matrix;
	glm::vec3 box_pos;
	glm::vec3 light_pos;
	bool rel_mode;
public:
	Win(const char *title, int res_x, int res_y, int offset_x,
		int offset_y, int gl_maj, int gl_min, unsigned int flags);
	~Win();
	void handle_resize();
	void handle_keyboard(std::string key);
	void handle_key_press(std::string key, bool pressed);
	void handle_mouse_motion(int x, int y);
	void display();
};

Win::Win(const char *title, int res_x, int res_y, int offset_x, int offset_y, int gl_maj, int gl_min, unsigned int flags) :
	sgltk::Window(title, res_x, res_y, offset_x, offset_y, gl_maj, gl_min, flags) {

	rel_mode = true;

	//create shaders
	display_shader.attach_file("display_vs.glsl", GL_VERTEX_SHADER);
	display_shader.attach_file("display_fs.glsl", GL_FRAGMENT_SHADER);
	display_shader.link();

	shadow_shader.attach_file("shadow_vs.glsl", GL_VERTEX_SHADER);
	shadow_shader.attach_file("shadow_fs.glsl", GL_FRAGMENT_SHADER);
	shadow_shader.link();

	floor_shader.attach_file("floor_vs.glsl", GL_VERTEX_SHADER);
	floor_shader.attach_file("floor_fs.glsl", GL_FRAGMENT_SHADER);
	floor_shader.link();

	box_shader.attach_file("box_vs.glsl", GL_VERTEX_SHADER);
	box_shader.attach_file("box_fs.glsl", GL_FRAGMENT_SHADER);
	box_shader.link();

	//create a camera
	camera = Camera(glm::vec3(0,10,20), glm::vec3(0,-0.125,-0.5),
			glm::vec3(0,1,0), 70.0f, (float)width,
			(float)height, 0.1f, 800.0f);

	std::vector<glm::vec4> pos = {	glm::vec4(-0.5, 0, -0.5, 1),
					glm::vec4( 0.5, 0, -0.5, 1),
					glm::vec4(-0.5, 0,  0.5, 1),
					glm::vec4( 0.5, 0,  0.5, 1)};

	std::vector<glm::vec3> norm = {	glm::vec3(0, 1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, 1, 0)};

	std::vector<glm::vec2> tex_coord = {	glm::vec2(0, 0),
						glm::vec2(1, 0),
						glm::vec2(0, 1),
						glm::vec2(1, 1)};

	std::vector<unsigned short> ind = {0, 1, 2, 2, 1, 3};

	depth_tex = Texture(GL_TEXTURE_2D, 1024, 1024);

	//create a plane
	int pos_loc_floor = floor_shader.get_attribute_location("pos_in");
	int norm_loc_floor = floor_shader.get_attribute_location("norm_in");
	int pos_loc_shadow = shadow_shader.get_attribute_location("pos_in");
	int norm_loc_shadow = shadow_shader.get_attribute_location("norm_in");
	int pos_buf = floor.attach_vertex_buffer<glm::vec4>(pos);
	int norm_buf = floor.attach_vertex_buffer<glm::vec3>(norm);
	floor.attach_index_buffer(ind);
	floor.set_vertex_attribute(pos_loc_floor, pos_buf, 4, GL_FLOAT, 0, 0);
	floor.set_vertex_attribute(norm_loc_floor, norm_buf, 3, GL_FLOAT, 0, 0);
	floor.set_vertex_attribute(pos_loc_shadow, pos_buf, 4, GL_FLOAT, 0, 0);
	floor.set_vertex_attribute(norm_loc_shadow, norm_buf, 3, GL_FLOAT, 0, 0);

	//create a mini display
	depth_display.setup_shader(&display_shader);
	pos_buf = depth_display.attach_vertex_buffer<glm::vec4>(pos);
	int tc_buf = depth_display.attach_vertex_buffer<glm::vec2>(tex_coord);
	depth_display.attach_index_buffer(ind);
	depth_display.set_vertex_attribute("pos_in", pos_buf, 4, GL_FLOAT, 0, 0);
	depth_display.set_vertex_attribute("tc_in", tc_buf, 2, GL_FLOAT, 0, 0);
	depth_display.model_matrix = glm::rotate((float)(M_PI / 2), glm::vec3(1, 0, 0));
	depth_display.textures_diffuse.push_back(&depth_tex);

	//load a model
	box_pos = glm::vec3(0, 4, 0);
	box.setup_shader(&box_shader);
	box.setup_camera(&camera.view_matrix, &camera.projection_matrix_persp);
	box.load("box.obj");

	light_pos = glm::vec3(5, 10, 0);
	light_cam = Camera(light_pos, box_pos - light_pos, glm::vec3(0, 1, 0),
			   70.0f, 1024, 1024, 0.1f, 800.0f);
	light_matrix = light_cam.projection_matrix_persp * light_cam.view_matrix;
	frame_buf.attach_texture(GL_DEPTH_ATTACHMENT, depth_tex);
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

	frame_buf.bind();	
	glm::mat4 mat = glm::translate(box_pos);
	box.setup_shader(&shadow_shader);
	box.setup_camera(&light_cam.view_matrix, &light_cam.projection_matrix_persp);
	box.draw(&mat);

	mat = glm::scale(glm::vec3(100.f, 1.f, 100.f));
	floor.setup_shader(&shadow_shader);
	floor.setup_camera(&light_cam.view_matrix, &light_cam.projection_matrix_persp);
	floor.draw(GL_TRIANGLES, &mat);
	frame_buf.unbind();


	box_shader.bind();
	box_shader.set_uniform("light_pos", light_pos);
	box_shader.set_uniform("cam_pos", camera.pos);
	box_shader.set_uniform("light_matrix", false, light_matrix);

	floor_shader.bind();
	floor_shader.set_uniform("light_pos", light_pos);
	floor_shader.set_uniform("cam_pos", camera.pos);
	floor_shader.set_uniform("light_matrix", false, light_matrix);

	mat = glm::translate(box_pos);
	box.setup_shader(&box_shader);
	box.draw(&mat);

	mat = glm::scale(glm::vec3(100.f, 1.f, 100.f));
	floor.setup_shader(&floor_shader);
	floor.setup_camera(&camera.view_matrix, &camera.projection_matrix_persp);
	floor.draw(GL_TRIANGLES, &mat);

	depth_display.draw(GL_TRIANGLES);
}

void Win::handle_keyboard(std::string key) {
	float mov_speed = 0.1f;
	float rot_speed = 0.05f;
	float dt = 1000 * (float)delta_time;
	if (dt < 2.0)
		dt = 2.0;

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

void Win::handle_mouse_motion(int x, int y) {
	if (rel_mode) {
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
	Scene::add_path("../data/models");
	Image::add_path("../data/textures");
	Shader::add_path("../shadow_mapping/shaders");

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
	Win window("Shadow mapping", w, h, x, y, 3, 0, 0);
	window.set_relative_mode(true);

	//start the mainloop
	window.run();
	return 0;
}
