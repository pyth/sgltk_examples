#include <sgltk/scene.h>
#include <sgltk/camera.h>
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
	Shader instanced_shadow_shader;
	P_Camera camera;
	O_Camera light_cam_o;
	P_Camera light_cam_p;
	Camera *curr_light_cam;
	Texture depth_tex;
	Texture floor_tex;
	Framebuffer frame_buf;

	glm::vec3 light_dir;
	bool rel_mode;
	bool perspective;
	std::vector<glm::mat4> model_matrix;
	void shadow_pass();
	void handle_resize();
	void handle_keyboard(const std::string& key);
	void handle_key_press(const std::string& key, bool pressed);
	void handle_mouse_motion(int x, int y);
	void display();
public:
	Win(const std::string& title, int res_x, int res_y, int offset_x,
		int offset_y, int gl_maj, int gl_min, unsigned int flags);
	~Win();
};

Win::Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y, int gl_maj, int gl_min, unsigned int flags) :
	sgltk::Window(title, res_x, res_y, offset_x, offset_y, gl_maj, gl_min, flags) {

	rel_mode = true;

	//create shaders
	display_shader.attach_file("display_vs.glsl", GL_VERTEX_SHADER);
	display_shader.attach_file("display_fs.glsl", GL_FRAGMENT_SHADER);
	display_shader.link();

	shadow_shader.attach_file("shadow_vs.glsl", GL_VERTEX_SHADER);
	shadow_shader.attach_file("shadow_fs.glsl", GL_FRAGMENT_SHADER);
	shadow_shader.link();

	instanced_shadow_shader.attach_file("instanced_shadow_vs.glsl", GL_VERTEX_SHADER);
	instanced_shadow_shader.attach_file("instanced_shadow_fs.glsl", GL_FRAGMENT_SHADER);
	instanced_shadow_shader.link();

	floor_shader.attach_file("floor_vs.glsl", GL_VERTEX_SHADER);
	floor_shader.attach_file("floor_fs.glsl", GL_FRAGMENT_SHADER);
	floor_shader.link();

	box_shader.attach_file("box_vs.glsl", GL_VERTEX_SHADER);
	box_shader.attach_file("box_fs.glsl", GL_FRAGMENT_SHADER);
	box_shader.link();

	//create a camera
	camera = P_Camera(glm::vec3(10,10,10), glm::vec3(-10, -8, -10),
			  glm::vec3(0,1,0), glm::radians(70.0f), (float)width,
			  (float)height, 0.1f, 800.0f);

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

	depth_tex.create_empty_2D(1024, 1024, GL_DEPTH_COMPONENT,
			       GL_FLOAT, GL_DEPTH_COMPONENT);

	floor_tex.load_texture("tile_sandstone_d.png");
	floor_tex.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	floor_tex.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	floor_tex.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_tex.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//create a floor plane
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

	//create a mini display
	depth_display.setup_shader(&display_shader);
	pos_buf = depth_display.attach_vertex_buffer<glm::vec4>(pos);
	tc_buf = depth_display.attach_vertex_buffer<glm::vec2>(tex_coord);
	depth_display.attach_index_buffer(ind);
	depth_display.set_vertex_attribute("pos_in", pos_buf, 4, GL_FLOAT, 0, 0);
	depth_display.set_vertex_attribute("tc_in", tc_buf, 2, GL_FLOAT, 0, 0);
	depth_display.textures_diffuse.push_back(&depth_tex);

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
	model_matrix[4] = glm::translate(glm::vec3(0, 2, -6)) * model_matrix[4];

	//load a model
	box.setup_shader(&box_shader);
	box.setup_camera(&camera);
	box.load("box.obj");
	box.attach_texture("shadow_map", &depth_tex);
	box.set_texture_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	box.set_texture_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	box.setup_instanced_matrix(model_matrix);

	light_dir = glm::vec3(-5, -14, 4);
	/*std::vector<glm::vec3> frustum(8);
	camera.calculate_frustum_points(&frustum[0], &frustum[1],
					&frustum[2], &frustum[3],
					&frustum[4], &frustum[5],
					&frustum[6], &frustum[7]);*/
	light_cam_o = O_Camera(-light_dir, light_dir, glm::vec3(0, 1, 0),
			       20, 20, 1, 100);
	light_cam_p = P_Camera(-light_dir, light_dir, glm::vec3(0, 1, 0),
			       glm::radians(90.f), 1024, 1024, 1, 100);
	curr_light_cam = &light_cam_p;
	perspective = true;
	frame_buf.attach_texture(GL_DEPTH_ATTACHMENT, depth_tex);
	frame_buf.finalize();
}

Win::~Win() {}

void Win::handle_resize() {
	glViewport(0, 0, width, height);
	camera.update_projection_matrix((float)width, (float)height);
}

void Win::shadow_pass() {
	frame_buf.bind();
	glViewport(0, 0, depth_tex.width, depth_tex.height);
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_FRONT);
	box.setup_shader(&instanced_shadow_shader);
	box.setup_camera(curr_light_cam);
	box.draw_instanced(5);
	glCullFace(GL_BACK);

	glm::mat4 mat = glm::scale(glm::vec3(100.f, 1.f, 100.f));
	floor.setup_shader(&shadow_shader);
	floor.setup_camera(curr_light_cam);
	floor.draw(GL_TRIANGLES, &mat);
	frame_buf.unbind();
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

	glm::mat4 light_matrix = curr_light_cam->projection_matrix * curr_light_cam->view_matrix;

	box_shader.bind();
	box_shader.set_uniform("light_dir", light_dir);
	box_shader.set_uniform("cam_pos", camera.pos);
	box_shader.set_uniform("light_matrix", false, light_matrix);
	box_shader.set_uniform_int("soft_shadow", 2);

	floor_shader.bind();
	floor_shader.set_uniform("light_dir", light_dir);
	floor_shader.set_uniform("cam_pos", camera.pos);
	floor_shader.set_uniform("light_matrix", false, light_matrix);
	floor_shader.set_uniform_int("soft_shadow", 2);

	box.setup_shader(&box_shader);
	box.setup_camera(&camera);
	box.draw_instanced(5);

	glm::mat4 mat = glm::scale(glm::vec3(100.f, 1.f, 100.f));
	floor.setup_shader(&floor_shader);
	floor.setup_camera(&camera);
	floor.draw(GL_TRIANGLES, &mat);

	display_shader.bind();
	if(perspective) {
		display_shader.set_uniform_int("perspective", 1);
	} else {
		display_shader.set_uniform_int("perspective", 0);
	}
	display_shader.set_uniform_float("near", curr_light_cam->near_plane);
	display_shader.set_uniform_float("far", curr_light_cam->far_plane);
	display_shader.set_uniform("resolution", glm::vec2(width, height));
	mat = glm::rotate((float)(M_PI / 2), glm::vec3(1, 0, 0));
	depth_display.draw(GL_TRIANGLES, &mat);
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
	} else if(key == "1") {
		curr_light_cam = &light_cam_o;
		perspective = false;
	} else if(key == "2") {
		curr_light_cam = &light_cam_p;
		perspective = true;
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
	if (rel_mode) {
		camera.yaw(-glm::atan((float)x) * dt);
		camera.pitch(-glm::atan((float)y) * dt);
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
