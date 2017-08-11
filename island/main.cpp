#include <sgltk/sgltk.h>

using namespace sgltk;

class Win : public Window {
	bool rel_mode;
	bool wireframe;
	int tile_size;
	unsigned int fps;
	unsigned int frame_cnt;
	unsigned int frame_sum;
	unsigned int terrain_side;
	unsigned int num_tiles;
	float terrain_max_height;
	float water_height;
	float sand_level;
	float sand_mix_level;
	float grass_level;
	float grass_mix_level;
	float rock_level;
	float rock_mix_level;

	glm::vec3 light_direction;

	std::vector<glm::vec2> tile_positions;

	glm::vec2 near_far;
	glm::mat4 light_matrix;

	Texture_2d height_map;
	Texture_2d water;
	Texture_2d sand;
	Texture_2d grass;
	Texture_2d rock;
	Texture_2d snow;
	Texture_2d color_tex;
	Texture_2d normal_tex;
	Texture_2d position_tex;
	Texture_2d spec_tex;
	Texture_2d refraction_tex;
	Texture_2d reflection_tex;
	Texture_2d shadow_tex;
	Texture_2d water_dudv;
	Texture_2d depth_tex;

	Framebuffer fb_shadow;
	Framebuffer fb_refract;
	Framebuffer fb_reflect;
	Framebuffer fb_normal;
	Renderbuffer depth_buffer;

	Buffer tile_buffer;
	Mesh water_mesh;
	Mesh display_mesh;
	Mesh terrain_tile;
	P_Camera camera;
	P_Camera reflection_cam;
	O_Camera shadow_cam;
	Shader water_shader;
	Shader terrain_shader;
	Shader terrain_refr_shader;
	Shader terrain_refl_shader;
	Shader terrain_shadow_shader;
	Shader display_shader;
	Timer timer;

	void handle_resize();
	void handle_key_press(const std::string& key, bool pressed);
	void handle_keyboard(const std::string& key);
	void handle_mouse_motion(int x, int y);
	void display();

	void shadow_pass();
	void reflect_pass();
	void refract_pass();
	void normal_pass();
	void calculate_shadow_frustum();
public:
	Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y);
	~Win();
};

Win::Win(const std::string& title, int res_x, int res_y, int offset_x, int offset_y) :
		Window(title, res_x, res_y, offset_x, offset_y) {

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
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

	terrain_max_height = 50.0f;
	water_height = 0.15f * terrain_max_height;
	sand_level = 0.2f * terrain_max_height;
	sand_mix_level = 0.25f * terrain_max_height;
	grass_level = 0.3f * terrain_max_height;
	grass_mix_level = 0.35f * terrain_max_height;
	rock_level = 0.4f * terrain_max_height;
	rock_mix_level = 0.45f * terrain_max_height;

	tile_size = 40;
	terrain_side = 50;
	num_tiles = static_cast<unsigned int>(pow(terrain_side, 2));

	std::vector<glm::vec4> position = {	glm::vec4(-0.5, 0,-0.5, 1),
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
				 (float)width, (float)height, 0.1f, 1000.0f);

	near_far = glm::vec2(camera.near_plane, camera.far_plane);

	reflection_cam = P_Camera(camera);
	reflection_cam.position = camera.position;
	reflection_cam.position[1] -= 2 * (reflection_cam.position[1] - water_height);
	reflection_cam.update_view_matrix();

	shadow_cam = O_Camera(glm::vec3(0), light_direction,
			      glm::vec3(0, 1, 0), 2048, 2048, 0.1, 1000);
	calculate_shadow_frustum();

	terrain_shader.attach_file("terrain_vs.glsl", GL_VERTEX_SHADER);
	terrain_shader.attach_file("terrain_tc.glsl", GL_TESS_CONTROL_SHADER);
	terrain_shader.attach_file("terrain_te.glsl", GL_TESS_EVALUATION_SHADER);
	terrain_shader.attach_file("terrain_fs.glsl", GL_FRAGMENT_SHADER);
	terrain_shader.link();

	terrain_shader.set_uniform_float("max_height", terrain_max_height);
	terrain_shader.set_uniform_float("sand_level", sand_level);
	terrain_shader.set_uniform_float("sand_mix_level", sand_mix_level);
	terrain_shader.set_uniform_float("grass_level", grass_level);
	terrain_shader.set_uniform_float("grass_mix_level", grass_mix_level);
	terrain_shader.set_uniform_float("rock_level", rock_level);
	terrain_shader.set_uniform_float("rock_mix_level", rock_mix_level);
	terrain_shader.set_uniform_int("tile_size", tile_size);
	terrain_shader.set_uniform_uint("terrain_side", terrain_side);
	terrain_shader.set_uniform_int("max_tess_level", App::sys_info.max_tess_level);

	terrain_refr_shader.attach_file("terrain_vs.glsl", GL_VERTEX_SHADER);
	terrain_refr_shader.attach_file("terrain_tc.glsl", GL_TESS_CONTROL_SHADER);
	terrain_refr_shader.attach_file("terrain_te.glsl", GL_TESS_EVALUATION_SHADER);
	terrain_refr_shader.attach_file("terrain_refr_fs.glsl", GL_FRAGMENT_SHADER);
	terrain_refr_shader.link();

	terrain_refr_shader.set_uniform_float("max_height", terrain_max_height);
	terrain_refr_shader.set_uniform_float("sand_level", sand_level);
	terrain_refr_shader.set_uniform_float("sand_mix_level", sand_mix_level);
	terrain_refr_shader.set_uniform_float("grass_level", grass_level);
	terrain_refr_shader.set_uniform_float("grass_mix_level", grass_mix_level);
	terrain_refr_shader.set_uniform_float("rock_level", rock_level);
	terrain_refr_shader.set_uniform_float("rock_mix_level", rock_mix_level);
	terrain_refr_shader.set_uniform_int("tile_size", tile_size);
	terrain_refr_shader.set_uniform_uint("terrain_side", terrain_side);
	terrain_refr_shader.set_uniform_int("max_tess_level", App::sys_info.max_tess_level);
	terrain_refr_shader.set_uniform("clip_plane", glm::vec4(0, -1, 0, 1.1 * water_height));

	terrain_refl_shader.attach_file("terrain_vs.glsl", GL_VERTEX_SHADER);
	terrain_refl_shader.attach_file("terrain_tc.glsl", GL_TESS_CONTROL_SHADER);
	terrain_refl_shader.attach_file("terrain_te.glsl", GL_TESS_EVALUATION_SHADER);
	terrain_refl_shader.attach_file("terrain_refl_fs.glsl", GL_FRAGMENT_SHADER);
	terrain_refl_shader.link();

	terrain_refl_shader.set_uniform_float("max_height", terrain_max_height);
	terrain_refl_shader.set_uniform_float("sand_level", sand_level);
	terrain_refl_shader.set_uniform_float("sand_mix_level", sand_mix_level);
	terrain_refl_shader.set_uniform_float("grass_level", grass_level);
	terrain_refl_shader.set_uniform_float("grass_mix_level", grass_mix_level);
	terrain_refl_shader.set_uniform_float("rock_level", rock_level);
	terrain_refl_shader.set_uniform_float("rock_mix_level", rock_mix_level);
	terrain_refl_shader.set_uniform_int("tile_size", tile_size);
	terrain_refl_shader.set_uniform_uint("terrain_side", terrain_side);
	terrain_refl_shader.set_uniform_int("max_tess_level", App::sys_info.max_tess_level);
	terrain_refl_shader.set_uniform("clip_plane", glm::vec4(0, 1, 0, -water_height));

	terrain_shadow_shader.attach_file("terrain_vs.glsl", GL_VERTEX_SHADER);
	terrain_shadow_shader.attach_file("terrain_tc.glsl", GL_TESS_CONTROL_SHADER);
	terrain_shadow_shader.attach_file("terrain_te.glsl", GL_TESS_EVALUATION_SHADER);
	terrain_shadow_shader.attach_file("terrain_shadow_fs.glsl", GL_FRAGMENT_SHADER);
	terrain_shadow_shader.link();

	terrain_shadow_shader.set_uniform_float("max_height", terrain_max_height);
	terrain_shadow_shader.set_uniform_int("tile_size", tile_size);
	terrain_shadow_shader.set_uniform_uint("terrain_side", terrain_side);
	terrain_shadow_shader.set_uniform_int("max_tess_level", App::sys_info.max_tess_level);

	display_shader.attach_file("display_vs.glsl", GL_VERTEX_SHADER);
	display_shader.attach_file("display_fs.glsl", GL_FRAGMENT_SHADER);
	display_shader.link();

	water_shader.attach_file("water_vs.glsl", GL_VERTEX_SHADER);
	water_shader.attach_file("water_fs.glsl", GL_FRAGMENT_SHADER);
	water_shader.link();

	height_map.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	height_map.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	height_map.load("island.jpg");

	water_dudv.load("terrain_water_dudv.jpg");
	water_dudv.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	water_dudv.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	water_dudv.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	water_dudv.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	water.load("terrain_water.jpg");
	water.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	water.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	water.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	water.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	sand.load("terrain_sand.jpg");
	sand.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	sand.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	grass.load("terrain_grass.jpg");
	grass.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	grass.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	rock.load("terrain_rock.jpg");
	rock.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	rock.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	snow.load("terrain_snow.jpg");
	snow.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	snow.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	depth_buffer.set_format(GL_DEPTH_COMPONENT);
	depth_buffer.set_size(width, height);

	color_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	normal_tex.create_empty(width, height, GL_RGB16F, GL_FLOAT, GL_RGB);
	position_tex.create_empty(width, height, GL_RGB16F, GL_FLOAT, GL_RGB);
	spec_tex.create_empty(width, height, GL_R32F, GL_FLOAT, GL_RED);
	depth_tex.create_empty(width, height, GL_R32F, GL_FLOAT, GL_RED);
	fb_normal.attach_texture(GL_COLOR_ATTACHMENT0, color_tex);
	fb_normal.attach_texture(GL_COLOR_ATTACHMENT1, normal_tex);
	fb_normal.attach_texture(GL_COLOR_ATTACHMENT2, position_tex);
	fb_normal.attach_texture(GL_COLOR_ATTACHMENT3, spec_tex);
	fb_normal.attach_renderbuffer(GL_DEPTH_ATTACHMENT, depth_buffer);
	fb_normal.finalize();

	refraction_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	fb_refract.attach_texture(GL_COLOR_ATTACHMENT0, refraction_tex);
	fb_refract.attach_texture(GL_COLOR_ATTACHMENT1, depth_tex);
	fb_refract.attach_renderbuffer(GL_DEPTH_ATTACHMENT, depth_buffer);
	fb_refract.finalize();

	reflection_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	fb_reflect.attach_texture(GL_COLOR_ATTACHMENT0, reflection_tex);
	fb_reflect.attach_renderbuffer(GL_DEPTH_ATTACHMENT, depth_buffer);
	fb_reflect.finalize();

	shadow_tex.create_empty(shadow_cam.width, shadow_cam.height, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_COMPONENT);
	fb_shadow.attach_texture(GL_DEPTH_ATTACHMENT, shadow_tex);
	fb_shadow.finalize();

	display_mesh.model_matrix = glm::rotate((float)(M_PI / 2), glm::vec3(1, 0, 0));
	display_mesh.setup_shader(&display_shader);
	display_mesh.setup_camera(&camera);
	display_mesh.textures_misc.push_back(std::make_pair("shadow_texture", &shadow_tex));
	display_mesh.textures_misc.push_back(std::make_pair("color_texture", &color_tex));
	display_mesh.textures_misc.push_back(std::make_pair("normal_texture", &normal_tex));
	display_mesh.textures_misc.push_back(std::make_pair("position_texture", &position_tex));
	display_mesh.textures_misc.push_back(std::make_pair("spec_texture", &spec_tex));
	display_mesh.textures_misc.push_back(std::make_pair("depth_texture", &depth_tex));
	display_mesh.add_vertex_attribute("pos_in", 4, GL_FLOAT, position);
	display_mesh.add_vertex_attribute("tc_in", 2, GL_FLOAT, tc);
	display_mesh.attach_index_buffer(ind);

	terrain_tile.setup_shader(&terrain_shader);
	terrain_tile.setup_camera(&camera);
	terrain_tile.add_vertex_attribute("vert_pos_in", 4, GL_FLOAT, position);
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
	terrain_tile.textures_misc.push_back(std::make_pair("sand_texture", &sand));
	terrain_tile.textures_misc.push_back(std::make_pair("grass_texture", &grass));
	terrain_tile.textures_misc.push_back(std::make_pair("rock_texture", &rock));
	terrain_tile.textures_misc.push_back(std::make_pair("snow_texture", &snow));
	terrain_tile.textures_misc.push_back(std::make_pair("shadow_texture", &shadow_tex));

	float water_size = terrain_side * tile_size * 1.1;
	water_mesh.model_matrix = glm::scale(glm::vec3(water_size, 1, water_size));
	water_mesh.model_matrix = glm::translate(glm::vec3(0, water_height, 0)) * water_mesh.model_matrix;
	water_mesh.setup_shader(&water_shader);
	water_mesh.setup_camera(&camera);
	water_mesh.add_vertex_attribute("pos_in", 4, GL_FLOAT, position);
	water_mesh.add_vertex_attribute("tc_in", 2, GL_FLOAT, tc);
	water_mesh.attach_index_buffer(ind);
	water_mesh.textures_misc.push_back(std::make_pair("water_texture", &water));
	water_mesh.textures_misc.push_back(std::make_pair("water_dudv_texture", &water_dudv));
	water_mesh.textures_misc.push_back(std::make_pair("depth_texture", &depth_tex));
	water_mesh.textures_misc.push_back(std::make_pair("refraction_texture", &refraction_tex));
	water_mesh.textures_misc.push_back(std::make_pair("reflection_texture", &reflection_tex));
	water_mesh.textures_misc.push_back(std::make_pair("shadow_texture", &shadow_tex));
}

Win::~Win() {
}

void Win::handle_resize() {
	glViewport(0, 0, width, height);
	depth_buffer.set_size(width, height);
	color_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	normal_tex.create_empty(width, height, GL_RGB16F, GL_FLOAT, GL_RGB);
	position_tex.create_empty(width, height, GL_RGB16F, GL_FLOAT, GL_RGB);
	spec_tex.create_empty(width, height, GL_RGB16F, GL_FLOAT, GL_RGB);
	depth_tex.create_empty(width, height, GL_RGB16F, GL_FLOAT, GL_RGB);
	refraction_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	reflection_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	camera.width = static_cast<float>(width);
	camera.width = static_cast<float>(height);
	camera.update_projection_matrix();
	reflection_cam.width = static_cast<float>(width);
	reflection_cam.width = static_cast<float>(height);
	reflection_cam.update_projection_matrix();
}

void Win::calculate_shadow_frustum() {
	std::vector<glm::vec3> frustum_points(8);
	camera.calculate_frustum_points(&frustum_points[0], &frustum_points[1],
					&frustum_points[2], &frustum_points[3],
					&frustum_points[4], &frustum_points[5],
					&frustum_points[6], &frustum_points[7]);

	glm::mat3 light_matrix = glm::lookAt(glm::vec3(0), glm::normalize(light_direction), glm::vec3(0, 1, 0));
	glm::mat3 light_matrix_inv = glm::inverse(light_matrix);

	for(int i = 0; i < 8; i++) {
		frustum_points[i] = light_matrix * frustum_points[i];
	}

	glm::vec3 min(frustum_points[0]);
	glm::vec3 max(frustum_points[0]);

	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 3; j++) {
			if(frustum_points[i][j] > max[j])
				max[j] = frustum_points[i][j];
			if(frustum_points[i][j] < min[j])
				min[j] = frustum_points[i][j];
		}
	}

	min = light_matrix_inv * min;
	max = light_matrix_inv * max;

	shadow_cam.position = glm::vec3(min[0] + 0.5 * (max[0] - min[0]), min[0] + 0.5 * (max[1] - min[1]), min[2]);
	shadow_cam.update_view_matrix();
	light_matrix = shadow_cam.projection_matrix * shadow_cam.view_matrix;
}

void Win::shadow_pass() {
	fb_shadow.bind();
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);

	terrain_tile.setup_camera(&shadow_cam);
	terrain_tile.setup_shader(&terrain_shadow_shader);
	terrain_tile.draw_instanced(GL_PATCHES, tile_positions.size());

	water_mesh.draw(GL_TRIANGLE_STRIP);
	fb_shadow.unbind();
}

void Win::reflect_pass() {
	fb_reflect.bind();
	glClearColor(0, 1, 1, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
	glDisable(GL_CULL_FACE);
	terrain_refl_shader.set_uniform("cam_pos", camera.position);
	terrain_refl_shader.set_uniform("light_direction", light_direction);
	terrain_refl_shader.set_uniform("light_matrix", false, light_matrix);
	terrain_tile.setup_camera(&reflection_cam);
	terrain_tile.setup_shader(&terrain_refl_shader);
	terrain_tile.draw_instanced(GL_PATCHES, tile_positions.size());
	fb_reflect.unbind();
}

void Win::refract_pass() {
	fb_refract.bind();
	glClearColor(0, 1, 1, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
	terrain_refr_shader.set_uniform("cam_pos", camera.position);
	terrain_refr_shader.set_uniform("light_direction", light_direction);
	terrain_refr_shader.set_uniform("light_matrix", false, light_matrix);
	terrain_tile.setup_camera(&camera);
	terrain_tile.setup_shader(&terrain_refr_shader);
	terrain_tile.draw_instanced(GL_PATCHES, tile_positions.size());
	fb_refract.unbind();
}

void Win::normal_pass() {
	fb_normal.bind();
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CLIP_DISTANCE0);
	terrain_shader.set_uniform("cam_pos", camera.position);
	terrain_shader.set_uniform("light_direction", light_direction);
	terrain_shader.set_uniform("clip_plane", glm::vec4(0, -1, 0, 0));
	terrain_tile.setup_camera(&camera);
	terrain_tile.setup_shader(&terrain_shader);
	terrain_tile.draw_instanced(GL_PATCHES, tile_positions.size());

	water_shader.set_uniform("near_far", near_far);
	water_shader.set_uniform("cam_pos", camera.position);
	water_shader.set_uniform("light_direction", light_direction);
	water_shader.set_uniform_float("time", timer.get_time_s());
	water_shader.set_uniform_uint("terrain_side", terrain_side);
	water_shader.set_uniform("light_matrix", false, light_matrix);
	water_mesh.draw(GL_TRIANGLE_STRIP);

	fb_normal.unbind();
	fb_normal.blit_to(NULL, 0, 0, width, height, 0, 0,
			   width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void Win::display() {
	glClearColor(0, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if(wireframe) {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	shadow_pass();
	reflect_pass();
	refract_pass();
	normal_pass();

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	display_shader.set_uniform("cam_pos", camera.position);
	display_shader.set_uniform("light_direction", light_direction);
	display_shader.set_uniform("light_matrix", false, light_matrix);
	display_mesh.draw(GL_TRIANGLE_STRIP);
}

void Win::handle_key_press(const std::string &key, bool pressed) {
	if(key == "Escape" && pressed) {
		stop();
	} else if(key == "P" && pressed) {
		display_shader.recompile();
		terrain_shader.recompile();
		terrain_refr_shader.recompile();
		terrain_refl_shader.recompile();
		terrain_shadow_shader.recompile();
		water_shader.recompile();

		terrain_shader.set_uniform_float("max_height", terrain_max_height);
		terrain_shader.set_uniform_float("sand_level", sand_level);
		terrain_shader.set_uniform_float("sand_mix_level", sand_mix_level);
		terrain_shader.set_uniform_float("grass_level", grass_level);
		terrain_shader.set_uniform_float("grass_mix_level", grass_mix_level);
		terrain_shader.set_uniform_float("rock_level", rock_level);
		terrain_shader.set_uniform_float("rock_mix_level", rock_mix_level);
		terrain_shader.set_uniform_int("tile_size", tile_size);
		terrain_shader.set_uniform_uint("terrain_side", terrain_side);
		terrain_shader.set_uniform_int("max_tess_level", App::sys_info.max_tess_level);

		terrain_refr_shader.set_uniform_float("max_height", terrain_max_height);
		terrain_refr_shader.set_uniform_float("sand_level", sand_level);
		terrain_refr_shader.set_uniform_float("sand_mix_level", sand_mix_level);
		terrain_refr_shader.set_uniform_float("grass_level", grass_level);
		terrain_refr_shader.set_uniform_float("grass_mix_level", grass_mix_level);
		terrain_refr_shader.set_uniform_float("rock_level", rock_level);
		terrain_refr_shader.set_uniform_float("rock_mix_level", rock_mix_level);
		terrain_refr_shader.set_uniform_int("tile_size", tile_size);
		terrain_refr_shader.set_uniform_uint("terrain_side", terrain_side);
		terrain_refr_shader.set_uniform_int("max_tess_level", App::sys_info.max_tess_level);
		terrain_refr_shader.set_uniform("clip_plane", glm::vec4(0, -1, 0, 1.1 * water_height));

		terrain_refl_shader.set_uniform_float("max_height", terrain_max_height);
		terrain_refl_shader.set_uniform_float("sand_level", sand_level);
		terrain_refl_shader.set_uniform_float("sand_mix_level", sand_mix_level);
		terrain_refl_shader.set_uniform_float("grass_level", grass_level);
		terrain_refl_shader.set_uniform_float("grass_mix_level", grass_mix_level);
		terrain_refl_shader.set_uniform_float("rock_level", rock_level);
		terrain_refl_shader.set_uniform_float("rock_mix_level", rock_mix_level);
		terrain_refl_shader.set_uniform_int("tile_size", tile_size);
		terrain_refl_shader.set_uniform_uint("terrain_side", terrain_side);
		terrain_refl_shader.set_uniform_int("max_tess_level", App::sys_info.max_tess_level);
		terrain_refl_shader.set_uniform("clip_plane", glm::vec4(0, 1, 0, -water_height));

		terrain_shadow_shader.set_uniform_float("max_height", terrain_max_height);
		terrain_shadow_shader.set_uniform_int("tile_size", tile_size);
		terrain_shadow_shader.set_uniform_uint("terrain_side", terrain_side);
		terrain_shadow_shader.set_uniform_int("max_tess_level", App::sys_info.max_tess_level);
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
		reflection_cam.roll(-rot_speed);
		update = true;
	} else if(key == "Q") {
		camera.roll(-rot_speed);
		reflection_cam.roll(rot_speed);
		update = true;
	}
	if(update) {
		if(camera.position[1] <= 1.1 * water_height)
			camera.position[1] = 1.1 * water_height;
		reflection_cam.position = camera.position;
		reflection_cam.position[1] -= 2 * (reflection_cam.position[1] - water_height);
		camera.update_view_matrix();
		reflection_cam.update_view_matrix();
		calculate_shadow_frustum();
	}
}

void Win::handle_mouse_motion(int x, int y) {
	if(rel_mode) {
		float rot_speed = (float)(2 * delta_time);
		camera.yaw(-glm::atan((float)x) * rot_speed);
		camera.pitch(-glm::atan((float)y) * rot_speed);
		camera.update_view_matrix();
		reflection_cam.yaw(-glm::atan((float)x) * rot_speed);
		reflection_cam.pitch(glm::atan((float)y) * rot_speed);
		reflection_cam.update_view_matrix();
		calculate_shadow_frustum();
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
