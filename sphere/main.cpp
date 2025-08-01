#include <sgltk/sgltk.h>

class Win : public sgltk::Window {
	bool rel_mode;
	bool wireframe;
	bool normals;

	glm::mat4 model_mat[2];
	float tess_level;
	glm::vec3 light_pos;

	sgltk::Shader normal_shader;
	sgltk::Shader sphere_shader;
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
	normals = true;
	set_relative_mode(rel_mode);

	glPatchParameteri(GL_PATCH_VERTICES, 3);

	model_mat[0] = glm::translate(glm::vec3(2, 0, 0));
	model_mat[1] = glm::translate(glm::vec3(-2, 0, 0));

	tess_level = 10;
	light_pos = glm::vec3(0, 2, 0);

	//compile and link the shaders
	sphere_shader.attach_file("sphere_vs.glsl", GL_VERTEX_SHADER);
	sphere_shader.attach_file("sphere_tc.glsl", GL_TESS_CONTROL_SHADER);
	sphere_shader.attach_file("sphere_te.glsl", GL_TESS_EVALUATION_SHADER);
	sphere_shader.attach_file("sphere_fs.glsl", GL_FRAGMENT_SHADER);
	sphere_shader.link();

	normal_shader.attach_file("normal_vs.glsl", GL_VERTEX_SHADER);
	normal_shader.attach_file("normal_tc.glsl", GL_TESS_CONTROL_SHADER);
	normal_shader.attach_file("normal_te.glsl", GL_TESS_EVALUATION_SHADER);
	normal_shader.attach_file("normal_gs.glsl", GL_GEOMETRY_SHADER);
	normal_shader.attach_file("normal_fs.glsl", GL_FRAGMENT_SHADER);
	normal_shader.link();

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

	sphere.attach_vertex_buffer(pos);
	sphere.attach_index_buffer(ind);
	sphere.setup_camera(&cam.view_matrix, &cam.projection_matrix);
	sphere.setup_shader(&sphere_shader);
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
	light.attach_vertex_buffer(light_vert);
	light.set_vertex_attribute("pos_in", 0, 4, GL_FLOAT, 0, 0);
	light.attach_index_buffer(light_ind);
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
	cam.width = static_cast<float>(width);
	cam.height = static_cast<float>(height);
	cam.update_projection_matrix();
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
	} else if(key == "N") {
		if(pressed) {
			normals = !normals;
		}
	} else if(key == "P") {
		if(pressed) {
			normal_shader.recompile();
			sphere_shader.recompile();
		}
	}
}

void Win::handle_keyboard(const std::string& key) {
	bool update = false;
	float mov_speed = 10.0f;
	float rot_speed = 2.0f;

	if(key == "D") {
		cam.move_right(mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "A") {
		cam.move_right(-mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "W") {
		cam.move_forward(mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "S") {
		cam.move_forward(-mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "R") {
		cam.move_up(mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "F") {
		cam.move_up(-mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "E") {
		cam.roll(rot_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "Q") {
		cam.roll(-rot_speed * static_cast<float>(delta_time));
		update = true;
	}
	if(update)
		cam.update_view_matrix();
}

void Win::handle_mouse_motion(int x, int y) {
	if(rel_mode) {
		cam.yaw(-glm::atan((float)x) * static_cast<float>(delta_time));
		cam.pitch(-glm::atan((float)y) * static_cast<float>(delta_time));
		cam.update_view_matrix();
	}
}

void Win::handle_mouse_wheel(int x, int y) {
	if(tess_level + y < 1)
		return;
	if(tess_level + y > sgltk::App::sys_info.max_tess_level)
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

	sphere_shader.set_uniform("tess_level", tess_level);
	sphere_shader.set_uniform("light_pos", light_pos);
	sphere.setup_shader(&sphere_shader);
	sphere.draw(GL_PATCHES, &model_mat[0]);
	sphere.draw(GL_PATCHES, &model_mat[1]);

	if(normals) {
		normal_shader.set_uniform("tess_level", tess_level);
		sphere.setup_shader(&normal_shader);
		sphere.draw(GL_PATCHES, &model_mat[0]);
		sphere.draw(GL_PATCHES, &model_mat[1]);
	}

	light_shader.set_uniform("light_pos", light_pos);
	light.draw(GL_POINTS);
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
	sgltk::App::set_msaa_sample_number(8);
	sgltk::Shader::add_path("../sphere/shaders");

	int w = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * sgltk::App::sys_info.display_bounds[0].h);
	int x = sgltk::App::sys_info.display_bounds[0].x +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].w);
	int y = sgltk::App::sys_info.display_bounds[0].y +
		(int)(0.125 * sgltk::App::sys_info.display_bounds[0].h);

	Win window("Tessellation sphere", w, h, x, y);
	if(window.gl_maj < 4)
		return -1;

	window.run(60);

	return 0;
}
