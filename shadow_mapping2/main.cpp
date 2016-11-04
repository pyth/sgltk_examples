#include <sgltk/scene.h>
#include <sgltk/window.h>
#include <sgltk/framebuffer.h>

#ifdef __linux__
	#include <unistd.h>
#else
	#include <direct.h>
#endif //__linux__
#include <string.h>

#define NUM_TILES 400

using namespace sgltk;

class Win : public Window {
	bool rel_mode;
	std::vector<glm::mat4> model_matrix;

	Scene box;
	Mesh floor;
	P_Camera camera;
	Texture floor_tex;
	Texture depth_tex;
	Shader box_shader;
	Shader floor_shader;
	Shader shadow_shader;
	Framebuffer frame_buf;

	void shadow_pass();
	void handle_resize();
	void handle_keyboard(const std::string& key);
	void handle_key_press(const std::string& key, bool pressed);
	void handle_mouse_motion(int x, int y);
	void display();
public:
	Win(const std::string& title,
	    int res_x, int res_y,
	    int offset_x, int offset_y,
	    int gl_maj, int gl_min,
	    unsigned int flags);
	~Win();
};

Win::Win(const std::string& title, int res_x, int res_y,
	 int offset_x, int offset_y, int gl_maj, int gl_min,
	 unsigned int flags) : Window(title, res_x, res_y,
	 offset_x, offset_y, gl_maj, gl_min, flags) {

	rel_mode = true;
	set_relative_mode(rel_mode);

	shadow_shader.attach_file("shadow_vs.glsl", GL_VERTEX_SHADER);
	shadow_shader.attach_file("shadow_fs.glsl", GL_FRAGMENT_SHADER);
	shadow_shader.link();

	floor_shader.attach_file("floor_vs.glsl", GL_VERTEX_SHADER);
	floor_shader.attach_file("floor_fs.glsl", GL_FRAGMENT_SHADER);
	floor_shader.link();

	box_shader.attach_file("box_vs.glsl", GL_VERTEX_SHADER);
	box_shader.attach_file("box_fs.glsl", GL_FRAGMENT_SHADER);
	box_shader.link();

	camera = P_Camera(glm::vec3(3, 7, 20), glm::vec3(0, 0, -1),
			  glm::vec3(0, 1, 0), glm::radians(70.0f),
			  (float)width, (float)height, 0.1f, 800.0f);

	std::vector<glm::vec4> pos = {		glm::vec4(-0.5, 0,  0.5, 1),
						glm::vec4( 0.5, 0,  0.5, 1),
						glm::vec4(-0.5, 0, -0.5, 1),
						glm::vec4( 0.5, 0, -0.5, 1)};

	std::vector<glm::vec3> norm = {		glm::vec3(0, 1, 0),
						glm::vec3(0, 1, 0),
						glm::vec3(0, 1, 0),
						glm::vec3(0, 1, 0)};

	std::vector<glm::vec2> tex_coord = {	glm::vec2(0, 0),
						glm::vec2(1, 0),
						glm::vec2(0, 1),
						glm::vec2(1, 1)};

	std::vector<unsigned short> ind = {0, 1, 2, 2, 1, 3};

	depth_tex.create_empty_cubemap(1024, 1024, GL_DEPTH_COMPONENT,
				       GL_FLOAT, GL_DEPTH_COMPONENT);

	floor_tex.load_texture("tile_sandstone_d.png");
	floor_tex.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	floor_tex.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	floor_tex.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_tex.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int pos_loc_floor = floor_shader.get_attribute_location("pos_in");
	int norm_loc_floor = floor_shader.get_attribute_location("norm_in");
	int tc_loc_floor = floor_shader.get_attribute_location("tc_in");
	int pos_buf = floor.attach_vertex_buffer<glm::vec4>(pos);
	int norm_buf = floor.attach_vertex_buffer<glm::vec3>(norm);
	int tc_buf = floor.attach_vertex_buffer<glm::vec2>(tex_coord);
	floor.attach_index_buffer(ind);
	floor.set_vertex_attribute(pos_loc_floor, pos_buf, 4, GL_FLOAT, 0, 0);
	floor.set_vertex_attribute(norm_loc_floor, norm_buf, 3, GL_FLOAT, 0, 0);
	floor.set_vertex_attribute(tc_loc_floor, tc_buf, 2, GL_FLOAT, 0, 0);
	floor.textures_diffuse.push_back(&floor_tex);
	floor.textures_misc.push_back(std::make_pair("shadow_map", &depth_tex));

	model_matrix.resize(5);
	model_matrix[0] = glm::rotate(glm::radians(0.0f), glm::vec3(0, 1, 0));
	model_matrix[0] = glm::translate(glm::vec3(0, 4, 0)) * model_matrix[0];
	model_matrix[1] = glm::rotate(glm::radians(14.0f), glm::vec3(0, 1, 1));
	model_matrix[1] = glm::translate(glm::vec3(2, 7, -2)) * model_matrix[1];
	model_matrix[2] = glm::rotate(glm::radians(24.0f), glm::vec3(0, 1, 0));
	model_matrix[2] = glm::translate(glm::vec3(-4, 4, -2)) * model_matrix[2];
	model_matrix[3] = glm::rotate(glm::radians(56.0f), glm::vec3(0, 0, 1));
	model_matrix[3] = glm::translate(glm::vec3(-6, 4, 2)) * model_matrix[3];
	model_matrix[4] = glm::rotate(glm::radians(78.0f), glm::vec3(1, 0, 0));
	model_matrix[4] = glm::translate(glm::vec3(4, 4, 1)) * model_matrix[4];

	//load a model
	box.setup_shader(&box_shader);
	box.setup_camera(&camera);
	box.load("box.obj");
	box.attach_texture("shadow_map", &depth_tex);
	box.set_texture_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	box.set_texture_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Win::~Win() {}

void Win::handle_resize() {
	glViewport(0, 0, width, height);
	camera.update_projection_matrix((float)width, (float)height);
}

void Win::handle_keyboard(const std::string& key) {
	float mov_speed = 0.1f;
	float rot_speed = 0.05f;
	float dt = 1000 * (float)delta_time;
	if(dt < 2.0)
		dt = 2.0;
	if(dt > 5.0)
		dt = 5.0;

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
	float dt = (float)delta_time;
	if(dt < 0.01)
		dt = 0.01f;
	if(dt > 1.0)
		dt = 1.0f;
	if(rel_mode) {
		camera.yaw(-glm::atan((float)x) * dt);
		camera.pitch(-glm::atan((float)y) * dt);
		camera.update_view_matrix();
	}
}

void Win::shadow_pass() {
}

void Win::display() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	shadow_pass();

	glViewport(0, 0, width, height);

	//clear the screen
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 light_dir = glm::vec3(-1, -1, 1);

	box_shader.bind();
	box_shader.set_uniform("light_dir", light_dir);
	box_shader.set_uniform("cam_pos", camera.pos);
	//box_shader.set_uniform("light_matrix", false, light_matrix);
	box_shader.set_uniform_int("soft_shadow", 5);

	floor_shader.bind();
	floor_shader.set_uniform("light_dir", light_dir);
	floor_shader.set_uniform("cam_pos", camera.pos);
	//floor_shader.set_uniform("light_matrix", false, light_matrix);
	floor_shader.set_uniform_int("soft_shadow", 5);

	box.setup_shader(&box_shader);
	box.setup_camera(&camera);
	for(glm::mat4 mat : model_matrix) {
		box.draw(&mat);
	}

	glm::mat4 mat = glm::scale(glm::vec3(100.f, 1.f, 100.f));
	floor.setup_shader(&floor_shader);
	floor.setup_camera(&camera);
	floor.draw(GL_TRIANGLES, &mat);
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

	App::init();

	Scene::add_path("../data/models");
	Image::add_path("../data/textures");
	Shader::add_path("../shadow_mapping/shaders");

	int w = (int)(0.75 * App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * App::sys_info.display_bounds[0].h);
	int x = App::sys_info.display_bounds[0].x +
		(int)(0.125 * App::sys_info.display_bounds[0].w);
	int y = App::sys_info.display_bounds[0].y +
		(int)(0.125 * App::sys_info.display_bounds[0].h);

	Win win("Shadow mapping 2", w, h, x, y, 3, 0, 0);

	win.run();

	return 0;
}
