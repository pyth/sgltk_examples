#include <sgltk/sgltk.h>

using namespace sgltk;

class Win : public sgltk::Window {
	bool rel_mode;
	std::vector<glm::mat4> model_matrix;

	glm::vec3 light_pos;
	glm::mat4 light_trafo;
	std::vector<glm::mat4> light_matrix;

	Mesh walls;
	Mesh light;
	Model box;
	Shader box_shader;
	Shader light_shader;
	Shader wall_shader;
	Shader shadow_shader;
	Shader shadow_inst_shader;
	Timer light_timer;
	P_Camera camera;
	P_Camera light_cam;
	Camera *curr_light_cam;
	Cubemap depth_tex;
	Cubemap wall_tex;
	Framebuffer frame_buf;

	void shadow_pass();
	void normal_pass();
	void handle_resize();
	void handle_keyboard(const std::string& key);
	void handle_key_press(const std::string& key, bool pressed);
	void handle_mouse_motion(int x, int y);
	void display();
public:
	Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y);
	~Win();
};

Win::Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y) :
		sgltk::Window(title, res_x, res_y, offset_x, offset_y) {

	rel_mode = true;
	set_relative_mode(rel_mode);
	light_matrix.resize(6);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);

	//create shaders
	light_shader.attach_string("#version 130\n"
				   "in vec4 pos_in;\n"
				   "uniform mat4 model_view_proj_matrix;\n"
				   "void main() {\n"
				   "gl_Position = model_view_proj_matrix * pos_in;\n"
				   "gl_PointSize = 10;\n"
				   "gl_FrontColor = vec4(1);}", GL_VERTEX_SHADER);
	light_shader.link();

	shadow_shader.attach_file("shadow_vs.glsl", GL_VERTEX_SHADER);
	shadow_shader.attach_file("shadow_gs.glsl", GL_GEOMETRY_SHADER);
	shadow_shader.attach_file("shadow_fs.glsl", GL_FRAGMENT_SHADER);
	shadow_shader.link();

	shadow_inst_shader.attach_file("shadow_inst_vs.glsl", GL_VERTEX_SHADER);
	shadow_inst_shader.attach_file("shadow_inst_gs.glsl", GL_GEOMETRY_SHADER);
	shadow_inst_shader.attach_file("shadow_inst_fs.glsl", GL_FRAGMENT_SHADER);
	shadow_inst_shader.link();

	wall_shader.attach_file("walls_vs.glsl", GL_VERTEX_SHADER);
	wall_shader.attach_file("walls_fs.glsl", GL_FRAGMENT_SHADER);
	wall_shader.link();

	box_shader.attach_file("box_vs.glsl", GL_VERTEX_SHADER);
	box_shader.attach_file("box_fs.glsl", GL_FRAGMENT_SHADER);
	box_shader.link();

	//create a camera
	camera = P_Camera(glm::vec3(-6, 8, 4), glm::vec3(0.7, -0.25, -0.6),
			  glm::vec3(0, 1, 0), glm::radians(70.0f), (float)width,
			  (float)height, 0.1f, 800.0f);

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

	std::vector<unsigned short> ind = {
		//front
		0, 1, 2, 2, 1, 3,
		//back
		6, 5, 4, 7, 5, 6,
		//left
		4, 0, 6, 6, 0, 2,
		//right
		1, 5, 7, 1, 7, 3,
		//bottom
		0, 4, 1, 1, 4, 5,
		//top
		2, 3, 6, 3, 7, 6
	};

	depth_tex.create_empty(1024, 1024, GL_DEPTH_COMPONENT,
				       GL_FLOAT, GL_DEPTH_COMPONENT);

	Image img("tile_sandstone_d.png");
	wall_tex.load(img, img, img, img, img, img);

	int pos_loc = wall_shader.get_attribute_location("pos_in");
	int pos_buf = walls.attach_vertex_buffer<glm::vec4>(pos);
	walls.attach_index_buffer(ind);
	walls.set_vertex_attribute(pos_loc, pos_buf, 4, GL_FLOAT, 0, 0);
	walls.model_matrix = glm::scale(glm::vec3(20.f, 20.f, 20.f));
	walls.textures_diffuse.push_back(&wall_tex);
	walls.textures_misc.push_back(std::make_pair("shadow_map", &depth_tex));

	std::vector<glm::vec4> light_point = {glm::vec4(0, 0, 0, 1)};
	std::vector<unsigned short> light_ind = {0};
	light.attach_vertex_buffer<glm::vec4>(light_point);
	light.set_vertex_attribute("pos_in", 0, 4, GL_FLOAT, 0, 0);
	light.attach_index_buffer(light_ind);
	light.setup_camera(&camera);
	light.setup_shader(&light_shader);

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

	light.model_matrix = glm::translate(glm::vec3(-2, 5, 0)) *
			     glm::translate(glm::vec3(0, 1 * cos(0), 0));
	light_pos = glm::vec3(light.model_matrix * glm::vec4(0, 0, 0, 1));
	light_cam = P_Camera(light_pos, glm::vec3(1, 0, 0), glm::vec3(0, 1, 0),
			     glm::radians(90.f), (float)depth_tex.width, (float)depth_tex.height, 1.0f, 50.f);

	light_cam.dir = glm::vec3(1, 0, 0);
	light_cam.up = glm::vec3(0, -1, 0);
	light_cam.update_view_matrix();
	light_matrix[0] = light_cam.projection_matrix * light_cam.view_matrix;
	light_cam.dir = glm::vec3(-1, 0, 0);
	light_cam.up = glm::vec3(0, -1, 0);
	light_cam.update_view_matrix();
	light_matrix[1] = light_cam.projection_matrix * light_cam.view_matrix;
	light_cam.dir = glm::vec3(0, 1, 0);
	light_cam.up = glm::vec3(0, 0, 1);
	light_cam.update_view_matrix();
	light_matrix[2] = light_cam.projection_matrix * light_cam.view_matrix;
	light_cam.dir = glm::vec3(0, -1, 0);
	light_cam.up = glm::vec3(0, 0, -1);
	light_cam.update_view_matrix();
	light_matrix[3] = light_cam.projection_matrix * light_cam.view_matrix;
	light_cam.dir = glm::vec3(0, 0, 1);
	light_cam.up = glm::vec3(0, -1, 0);
	light_cam.update_view_matrix();
	light_matrix[4] = light_cam.projection_matrix * light_cam.view_matrix;
	light_cam.dir = glm::vec3(0, 0, -1);
	light_cam.up = glm::vec3(0, -1, 0);
	light_cam.update_view_matrix();
	light_matrix[5] = light_cam.projection_matrix * light_cam.view_matrix;

	frame_buf.attach_texture(GL_DEPTH_ATTACHMENT, depth_tex);
	frame_buf.finalize();

	light_timer.start();
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

	shadow_inst_shader.set_uniform("light_pos", light_pos);
	shadow_inst_shader.set_uniform("light_matrix", false, light_matrix);
	shadow_inst_shader.set_uniform_float("far_plane", light_cam.far_plane);

	box.setup_shader(&shadow_inst_shader);
	box.set_instanced_matrix_attributes();
	box.setup_camera(&light_cam);
	box.draw_instanced(5);
	glCullFace(GL_BACK);

	frame_buf.unbind();
}

void Win::normal_pass() {
	glViewport(0, 0, width, height);

	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	box_shader.set_uniform_float("far_plane", light_cam.far_plane);
	box_shader.set_uniform("light_pos", light_pos);
	box_shader.set_uniform("cam_pos", camera.pos);
	box_shader.set_uniform("light_matrix", false, light_matrix);
	box_shader.set_uniform_int("soft_shadow", 1);

	wall_shader.set_uniform_float("far_plane", light_cam.far_plane);
	wall_shader.set_uniform("light_pos", light_pos);
	wall_shader.set_uniform("cam_pos", camera.pos);
	wall_shader.set_uniform("light_matrix", false, light_matrix);
	wall_shader.set_uniform_int("soft_shadow", 1);

	box.setup_shader(&box_shader);
	box.set_instanced_matrix_attributes();
	box.setup_camera(&camera);
	box.draw_instanced(5);

	walls.setup_shader(&wall_shader);
	walls.setup_camera(&camera);
	walls.draw(GL_TRIANGLES);

	light.draw(GL_POINTS);
}

void Win::display() {

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	light.model_matrix = glm::translate(glm::vec3(-2, 5, 0)) *
			     glm::translate(glm::vec3(0, 2 * cos(M_PI * light_timer.get_time_s()), 0));
	light_pos = glm::vec3(light.model_matrix * glm::vec4(0, 0, 0, 1));
	light_cam.pos = light_pos;

	light_cam.dir = glm::vec3(1, 0, 0);
	light_cam.up = glm::vec3(0, -1, 0);
	light_cam.update_view_matrix();
	light_matrix[0] = light_cam.projection_matrix * light_cam.view_matrix;
	light_cam.dir = glm::vec3(-1, 0, 0);
	light_cam.up = glm::vec3(0, -1, 0);
	light_cam.update_view_matrix();
	light_matrix[1] = light_cam.projection_matrix * light_cam.view_matrix;
	light_cam.dir = glm::vec3(0, 1, 0);
	light_cam.up = glm::vec3(0, 0, 1);
	light_cam.update_view_matrix();
	light_matrix[2] = light_cam.projection_matrix * light_cam.view_matrix;
	light_cam.dir = glm::vec3(0, -1, 0);
	light_cam.up = glm::vec3(0, 0, -1);
	light_cam.update_view_matrix();
	light_matrix[3] = light_cam.projection_matrix * light_cam.view_matrix;
	light_cam.dir = glm::vec3(0, 0, 1);
	light_cam.up = glm::vec3(0, -1, 0);
	light_cam.update_view_matrix();
	light_matrix[4] = light_cam.projection_matrix * light_cam.view_matrix;
	light_cam.dir = glm::vec3(0, 0, -1);
	light_cam.up = glm::vec3(0, -1, 0);
	light_cam.update_view_matrix();
	light_matrix[5] = light_cam.projection_matrix * light_cam.view_matrix;

	shadow_pass();
	glCullFace(GL_BACK);
	normal_pass();
}

void Win::handle_keyboard(const std::string& key) {
	bool update = false;
	float mov_speed = 20.0f;
	float rot_speed = 2.0f;

	if(key == "D") {
		camera.move_right(mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "A") {
		camera.move_right(-mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "W") {
		camera.move_forward(mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "S") {
		camera.move_forward(-mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "R") {
		camera.move_up(mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "F") {
		camera.move_up(-mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "E") {
		camera.roll(rot_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "Q") {
		camera.roll(-rot_speed * static_cast<float>(delta_time));
		update = true;
	}
	if(update)
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
	if (rel_mode) {
		camera.yaw(-glm::atan((float)x) * static_cast<float>(delta_time));
		camera.pitch(-glm::atan((float)y) * static_cast<float>(delta_time));
		camera.update_view_matrix();
	}
}

int main(int argc, char **argv) {
	//initialize the library
	App::init();
	//change the current working directory to the one containing the executable
	App::chdir_to_bin(argv);

	//set the pathes to search for assets and shaders
	Model::add_path("../data/models");
	Image::add_path("../data/textures");
	Shader::add_path("../shadow_mapping2/shaders");

	int w = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].h);

	//open a window
	Win window("Shadow mapping 2", w, h, x, y);
	if(window.gl_maj == 3 && window.gl_min < 2)
		return -1;

	//start the mainloop
	window.run(100);
	return 0;
}
