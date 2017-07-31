#include <sgltk/sgltk.h>

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
	Texture_2d color_tex;
	Texture_2d normal_tex;
	Texture_2d position_tex;
	Framebuffer framebuffer;
	Renderbuffer depth_buffer;
	Buffer tile_buffer;
	Mesh display_mesh;
	Mesh terrain_tile;
	P_Camera camera;
	Shader terrain_shader;
	Shader display_shader;

	void handle_key_press(const std::string& key, bool pressed);
	void handle_keyboard(const std::string& key);
	void handle_mouse_motion(int x, int y);
	void display();
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

	tile_size = 40;
	tile_radius = static_cast<float>(sqrt(2 * pow(tile_size, 2)));
	terrain_side = 50;
	num_tiles = static_cast<unsigned int>(pow(terrain_side, 2));

	std::vector<glm::vec4> pos = {	glm::vec4(-0.5, 0,-0.5, 1),
					glm::vec4(-0.5, 0, 0.5, 1),
					glm::vec4( 0.5, 0,-0.5, 1),
					glm::vec4( 0.5, 0, 0.5, 1)};

	std::vector<glm::vec2> tc = {	glm::vec2(0, 0),
					glm::vec2(0, 1),
					glm::vec2(1, 0),
					glm::vec2(1, 1)};

	std::vector<unsigned short> ind = {0, 1, 2, 3};

	camera = P_Camera(glm::vec3(0, 40, 0), glm::vec3(0, 0, -1),
				 glm::vec3(0, 1, 0), glm::radians(70.f),
				 (float)width, (float)height, 0.1f, 2000.0f);

	terrain_shader.attach_file("terrain_vs.glsl", GL_VERTEX_SHADER);
	terrain_shader.attach_file("terrain_tc.glsl", GL_TESS_CONTROL_SHADER);
	terrain_shader.attach_file("terrain_te.glsl", GL_TESS_EVALUATION_SHADER);
	terrain_shader.attach_file("terrain_fs.glsl", GL_FRAGMENT_SHADER);
	terrain_shader.link();

	display_shader.attach_file("display_vs.glsl", GL_VERTEX_SHADER);
	display_shader.attach_file("display_fs.glsl", GL_FRAGMENT_SHADER);
	display_shader.link();

	height_map.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	height_map.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

	color_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	normal_tex.create_empty(width, height, GL_RGB16F, GL_FLOAT, GL_RGB);
	position_tex.create_empty(width, height, GL_RGB16F, GL_FLOAT, GL_RGB);
	depth_buffer.set_format(GL_DEPTH_COMPONENT);
	depth_buffer.set_size(width, height);
	framebuffer.attach_texture(GL_COLOR_ATTACHMENT0, color_tex);
	framebuffer.attach_texture(GL_COLOR_ATTACHMENT1, normal_tex);
	framebuffer.attach_texture(GL_COLOR_ATTACHMENT2, position_tex);
	framebuffer.attach_renderbuffer(GL_DEPTH_ATTACHMENT, depth_buffer);
	framebuffer.finalize();

	terrain_tile.setup_shader(&terrain_shader);
	terrain_tile.setup_camera(&camera);
	terrain_tile.add_vertex_attribute("vert_pos_in", 4, GL_FLOAT, pos);
	terrain_tile.attach_index_buffer(ind);

	display_mesh.model_matrix = glm::rotate(glm::mat4(1), (float)(M_PI / 2), glm::vec3(1, 0, 0));
	display_mesh.setup_shader(&display_shader);
	display_mesh.setup_camera(&camera);
	display_mesh.textures_misc.push_back(std::make_pair("color_texture", &color_tex));
	display_mesh.textures_misc.push_back(std::make_pair("normal_texture", &normal_tex));
	display_mesh.textures_misc.push_back(std::make_pair("position_texture", &position_tex));
	display_mesh.add_vertex_attribute("pos_in", 4, GL_FLOAT, pos);
	display_mesh.add_vertex_attribute("tc_in", 2, GL_FLOAT, tc);
	display_mesh.attach_index_buffer(ind);

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
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if(wireframe) {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	framebuffer.bind();
	glDisable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	terrain_shader.set_uniform_float("max_height", 50);
	terrain_shader.set_uniform_int("tile_size", tile_size);
	terrain_shader.set_uniform_uint("terrain_side", terrain_side);
	terrain_shader.set_uniform_int("max_tess_level",
				App::sys_info.max_tess_level);
	terrain_tile.draw_instanced(GL_PATCHES, tile_positions.size());
	framebuffer.unbind();
	framebuffer.blit_to(NULL, 0, 0, width, height, 0, 0,
			    width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glEnable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	display_shader.set_uniform("cam_pos", camera.pos);
	display_shader.set_uniform("light_direction", light_direction);
	display_mesh.draw(GL_TRIANGLE_STRIP);
}

void Win::handle_key_press(const std::string &key, bool pressed) {
	if(key == "Escape" && pressed) {
		stop();
	} else if(key == "P" && pressed) {
		display_shader.recompile();
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
	bool update = false;
	float mov_speed = (float)(100 * delta_time);
	float rot_speed = (float)(5 * delta_time);

	if(key == "D") {
		camera.move_right(mov_speed);
		update = true;
	} else if(key == "A") {
		camera.move_right(-mov_speed);
		update = true;
	} else if(key == "W") {
		camera.move_forward(mov_speed);
		update = true;
	} else if(key == "S") {
		camera.move_forward(-mov_speed);
		update = true;
	} else if(key == "R") {
		camera.move_up(mov_speed);
		update = true;
	} else if(key == "F") {
		camera.move_up(-mov_speed);
		update = true;
	} else if(key == "E") {
		camera.roll(rot_speed);
		update = true;
	} else if(key == "Q") {
		camera.roll(-rot_speed);
		update = true;
	}
	if(update)
		camera.update_view_matrix();
}

void Win::handle_mouse_motion(int x, int y) {
	if(rel_mode) {
		float rot_speed = (float)(2 * delta_time);
		camera.yaw(-glm::atan((float)x) * rot_speed);
		camera.pitch(-glm::atan((float)y) * rot_speed);
		camera.update_view_matrix();
	}
}

int main(int argc, char **argv) {
	App::init();
	App::chdir_to_bin(argv);

	Shader::add_path("../island/shaders");
	Texture::add_path("../data/textures");

	int w = (int)(0.75 * App::sys_info.display_bounds[0].w);
	int h = (int)(0.75 * App::sys_info.display_bounds[0].h);
	int x = App::sys_info.display_bounds[0].x +
		(int)(0.125 * App::sys_info.display_bounds[0].w);
	int y = App::sys_info.display_bounds[0].y +
		(int)(0.125 * App::sys_info.display_bounds[0].h);

	Win window("Island", w, h, x, y);
	if(window.gl_maj < 3)
		return -1;

	window.run(100);

	return 0;
}
