#include <sgltk/app.h>
#include <sgltk/window.h>
#include <sgltk/shader.h>
#include <sgltk/camera.h>
#include <sgltk/mesh.h>

#ifdef __linux__
	#include <unistd.h>
#else
	#include <direct.h>
#endif //__linux__

using namespace sgltk;

class Win : public Window {
	bool rel_mode;
	bool wireframe;
	int tile_size;
	unsigned int fps;
	unsigned int frame_cnt;
	unsigned int frame_sum;
	float tile_radius;
	unsigned int terrain_side;
	unsigned int num_tiles;

	glm::vec3 light_direction;

	std::vector<glm::vec2> tile_positions;
	std::vector<glm::vec2> tiles_to_draw;

	Texture_2d height_map;
	Texture_2d water;
	Texture_2d sand;
	Texture_2d grass;
	Texture_2d rock;
	Texture_2d snow;
	Buffer tile_buffer;
	Mesh terrain_tile;
	P_Camera camera;
	Shader terrain_shader;

	void handle_key_press(const std::string& key, bool pressed);
	void handle_keyboard(const std::string& key);
	void handle_mouse_motion(int x, int y);
	void display();

	void frustum_culling();
public:
	Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y);
	~Win();
};

Win::Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y) :
		Window(title, res_x, res_y, offset_x, offset_y) {

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	fps = 0;
	frame_cnt = 0;
	frame_sum = 0;
	rel_mode = true;
	wireframe = false;

	set_relative_mode(rel_mode);

	light_direction = glm::vec3(-10, -10, -10);

	tile_size = 20;
	tile_radius = sqrt(2 * pow(tile_size, 2));
	terrain_side = 50;
	num_tiles = pow(terrain_side, 2);

	std::vector<glm::vec4> pos = {	glm::vec4(-0.5, 0,-0.5, 1),
					glm::vec4(-0.5, 0, 0.5, 1),
					glm::vec4( 0.5, 0,-0.5, 1),
					glm::vec4( 0.5, 0, 0.5, 1)};

	std::vector<unsigned short> ind = {0, 1, 2, 3};

	camera = P_Camera(glm::vec3(0, 40, 0), glm::vec3(0, 0, -1),
				 glm::vec3(0, 1, 0), glm::radians(70.f),
				 (float)width, (float)height, 0.1f, 2000.0f);

	terrain_shader.attach_file("terrain_vs.glsl", GL_VERTEX_SHADER);
	terrain_shader.attach_file("terrain_tc.glsl", GL_TESS_CONTROL_SHADER);
	terrain_shader.attach_file("terrain_te.glsl", GL_TESS_EVALUATION_SHADER);
	terrain_shader.attach_file("terrain_fs.glsl", GL_FRAGMENT_SHADER);
	terrain_shader.link();

	height_map.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NONE);
	height_map.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NONE);
	height_map.load("island.jpg");

	water.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	water.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	water.load("terrain_water.jpg");
	sand.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	sand.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	sand.load("terrain_sand.jpg");
	grass.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	grass.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	grass.load("terrain_grass.jpg");
	rock.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	rock.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	rock.load("terrain_rock.jpg");
	snow.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	snow.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	snow.load("terrain_snow.jpg");

	terrain_tile.setup_shader(&terrain_shader);
	terrain_tile.setup_camera(&camera);
	terrain_tile.add_vertex_attribute("vert_pos_in", 4, GL_FLOAT, pos);
	terrain_tile.attach_index_buffer(ind);

	tile_positions.resize(num_tiles);
	for(unsigned int i = 0; i < terrain_side; i++) {
		for(unsigned int j = 0; j < terrain_side; j++) {
			tile_positions[i * terrain_side + j] =
				glm::vec2(i, j);
		}
	}
	tile_buffer.load(tile_positions, GL_DYNAMIC_DRAW);
	terrain_tile.set_buffer_vertex_attribute("tile_pos_in", &tile_buffer, 2, GL_FLOAT, 0, 0, 1);
	terrain_tile.textures_displacement.push_back(&height_map);
	terrain_tile.textures_misc.push_back(std::make_pair("water_texture", &water));
	terrain_tile.textures_misc.push_back(std::make_pair("sand_texture", &sand));
	terrain_tile.textures_misc.push_back(std::make_pair("grass_texture", &grass));
	terrain_tile.textures_misc.push_back(std::make_pair("rock_texture", &rock));
	terrain_tile.textures_misc.push_back(std::make_pair("snow_texture", &snow));
}

Win::~Win() {
}

void Win::display() {
	frame_cnt++;
	frame_sum += (unsigned int)(1.0 / delta_time);
	if(frame_cnt > 100) {
		fps = (unsigned int)(frame_sum / frame_cnt);
		frame_cnt = 0;
		frame_sum = 0;
	}
	set_title("Island " + std::to_string(fps));

	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if(wireframe) {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	terrain_shader.set_uniform_float("max_height", 30);
	terrain_shader.set_uniform("cam_pos", camera.pos);
	terrain_shader.set_uniform("light_direction", light_direction);
	terrain_shader.set_uniform_int("tile_size", tile_size);
	terrain_shader.set_uniform_uint("terrain_side", terrain_side);
	terrain_shader.set_uniform_int("max_tess_level",
				App::sys_info.max_tess_level);
	terrain_tile.draw_instanced(GL_PATCHES, tile_positions.size());
}

void Win::handle_key_press(const std::string &key, bool pressed) {
	if(key == "Escape" && pressed) {
		stop();
	} else if(key == "P" && pressed) {
		terrain_shader.recompile();
	} else if(key == "L") {
		if(pressed) {
			wireframe = !wireframe;
		}
	} else if(key == "M") {
		if(pressed) {
			rel_mode = !rel_mode;
			set_relative_mode(rel_mode);
		}
	}
}

void Win::handle_keyboard(const std::string& key) {
	float dt = 1000 * (float)delta_time;
	float mov_speed = 1.f * dt;
	float rot_speed = 0.01f * dt;
	if(mov_speed < 0.8)
		mov_speed = 0.8;
	if(mov_speed > 1.0)
		mov_speed = 1.0;
	if(rot_speed < 0.05)
		rot_speed = 0.05;
	if(rot_speed > 0.3)
		rot_speed = 0.3;

	if(key == "D") {
		camera.move_right(mov_speed);
	} else if(key == "A") {
		camera.move_right(-mov_speed);
	} else if(key == "W") {
		camera.move_forward(mov_speed);
	} else if(key == "S") {
		camera.move_forward(-mov_speed);
	} else if(key == "R") {
		camera.move_up(mov_speed);
	} else if(key == "F") {
		camera.move_up(-mov_speed);
	} else if(key == "E") {
		camera.roll(rot_speed);
	} else if(key == "Q") {
		camera.roll(-rot_speed);
	}
	camera.update_view_matrix();
}

void Win::handle_mouse_motion(int x, int y) {
	if(rel_mode) {
		float dt = 10 * (float)delta_time;
		if(dt > 0.01)
			dt = 0.01f;
		camera.yaw(-glm::atan((float)x) * dt);
		camera.pitch(-glm::atan((float)y) * dt);
		camera.update_view_matrix();
	}
}

int main(int argc, char **argv) {
	std::string path(argv[0]);
	path = path.substr(0, path.find_last_of("\\/"));
#ifdef __linux__
	chdir(path.c_str());
#else
	_chdir(path.c_str());
#endif

	App::init();

	App::set_gl_version(4, 0);

	Shader::add_path("../island/shaders");
	Texture::add_path("../data/textures");

	int w = (int)(0.75 * App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * App::sys_info.display_bounds[0].h);
	int x = App::sys_info.display_bounds[0].x +
		(int)(0.125 * App::sys_info.display_bounds[0].w);
	int y = App::sys_info.display_bounds[0].y +
		(int)(0.125 * App::sys_info.display_bounds[0].h);

	Win window("Island 0", w, h, x, y);

	window.run();

	return 0;
}
