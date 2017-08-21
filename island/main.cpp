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
	double time;
	float terrain_max_height;
	float water_height;
	float sand_level;
	float sand_mix_level;
	float grass_level;
	float grass_mix_level;
	float rock_level;
	float rock_mix_level;

	glm::vec3 shadow_dist;
	glm::vec3 light_direction;

	std::vector<glm::vec2> tile_positions;

	glm::vec2 near_far;
	std::vector<glm::mat4> light_matrix;

	Texture_2d height_map;
	Texture_2d sand;
	Texture_2d grass;
	Texture_2d rock;
	Texture_2d snow;
	Texture_2d color_tex;
	Texture_2d normal_tex;
	Texture_2d position_tex;
	Texture_2d spec_tex;
	Texture_2d depth_tex;
	Texture_2d refraction_tex;
	Texture_2d reflection_tex;
	Texture_2d shadow_tex;
	std::vector<Texture_2d> shadow_map;
	Texture_2d water_dudv;
	Cubemap sky_tex;

	std::vector<Framebuffer> fb_shadow;
	Framebuffer fb_refract;
	Framebuffer fb_reflect;
	Framebuffer fb_color;
	Renderbuffer depth_buffer;

	Buffer tile_buffer;
	Mesh water_mesh;
	Mesh display_mesh;
	Mesh terrain_tile;
	Mesh skybox;
	P_Camera camera;
	P_Camera reflection_cam;
	std::vector<O_Camera> shadow_cam;
	IP_Camera ip_cam;
	IP_Camera refl_ip_cam;
	Shader water_shader;
	Shader terrain_shader;
	Shader terrain_refr_shader;
	Shader terrain_refl_shader;
	Shader terrain_shadow_shader;
	Shader display_shader;
	Shader skybox_shader;
	Timer timer;

	void handle_resize();
	void handle_key_press(const std::string& key, bool pressed);
	void handle_keyboard(const std::string& key);
	void handle_mouse_motion(int x, int y);
	void display();

	void shadow_pass();
	void reflect_pass();
	void refract_pass();
	void color_pass();
	void position_pass();
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

	fb_shadow.resize(3);
	shadow_map.resize(3);
	shadow_cam.resize(3);
	light_matrix.resize(3);
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

	std::vector<glm::vec4> skybox_pos = {
		glm::vec4(-1, -1, -1, 1),
		glm::vec4( 1, -1, -1, 1),
		glm::vec4(-1,  1, -1, 1),
		glm::vec4( 1,  1, -1, 1),

		glm::vec4(-1, -1,  1, 1),
		glm::vec4( 1, -1,  1, 1),
		glm::vec4(-1,  1,  1, 1),
		glm::vec4( 1,  1,  1, 1)
	};

	std::vector<unsigned short> skybox_ind = {
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

	camera = P_Camera(glm::vec3(0, 20, 0), glm::vec3(0, 0, -1),
				 glm::vec3(0, 1, 0), glm::radians(70.f),
				 (float)width, (float)height, 0.1f, 800.0f);

	near_far = glm::vec2(camera.near_plane, camera.far_plane);
	shadow_dist = glm::vec3(40, 200, camera.far_plane);

	reflection_cam = P_Camera(camera);
	reflection_cam.position = camera.position;
	reflection_cam.position[1] -= 2 * (reflection_cam.position[1] - water_height);
	reflection_cam.direction = camera.direction;
	reflection_cam.direction[1] *= -1;
	reflection_cam.update_view_matrix();

	ip_cam = IP_Camera(glm::vec3(0, 20, 0), glm::vec3(0, 0, -1),
				 glm::vec3(0, 1, 0), glm::radians(70.f),
				 (float)width, (float)height, 0.1f);

	for(unsigned int i = 0; i < shadow_cam.size(); i++) {
		shadow_cam[i] = O_Camera(glm::vec3(0), light_direction,
					 glm::vec3(0, 1, 0), (float)width, (float)height, 0.1, 100);
	}

	refl_ip_cam.position = reflection_cam.position;
	refl_ip_cam.direction = reflection_cam.direction;
	refl_ip_cam.up = reflection_cam.up;
	refl_ip_cam.fovy = reflection_cam.fovy;
	refl_ip_cam.width = reflection_cam.width;
	refl_ip_cam.height = reflection_cam.height;
	refl_ip_cam.near_plane = reflection_cam.near_plane;
	refl_ip_cam.update_view_matrix();
	refl_ip_cam.update_projection_matrix();

	terrain_shader.attach_file("terrain_vs.glsl", GL_VERTEX_SHADER);
	terrain_shader.attach_file("terrain_tc.glsl", GL_TESS_CONTROL_SHADER);
	terrain_shader.attach_file("terrain_te.glsl", GL_TESS_EVALUATION_SHADER);
	terrain_shader.attach_file("terrain_fs.glsl", GL_FRAGMENT_SHADER);
	terrain_shader.link();

	terrain_shader.set_uniform("shadow_distance", shadow_dist);
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
	terrain_shader.set_uniform("cam_pos", camera.position);
	terrain_shader.set_uniform("light_direction", light_direction);
	terrain_shader.set_uniform("clip_plane", glm::vec4(0, -1, 0, 0));

	terrain_refr_shader.attach_file("terrain_vs.glsl", GL_VERTEX_SHADER);
	terrain_refr_shader.attach_file("terrain_tc.glsl", GL_TESS_CONTROL_SHADER);
	terrain_refr_shader.attach_file("terrain_te.glsl", GL_TESS_EVALUATION_SHADER);
	terrain_refr_shader.attach_file("terrain_refr_fs.glsl", GL_FRAGMENT_SHADER);
	terrain_refr_shader.link();

	terrain_refr_shader.set_uniform("shadow_distance", shadow_dist);
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
	terrain_refr_shader.set_uniform("cam_pos", camera.position);
	terrain_refr_shader.set_uniform("light_direction", light_direction);

	terrain_refl_shader.attach_file("terrain_vs.glsl", GL_VERTEX_SHADER);
	terrain_refl_shader.attach_file("terrain_tc.glsl", GL_TESS_CONTROL_SHADER);
	terrain_refl_shader.attach_file("terrain_te.glsl", GL_TESS_EVALUATION_SHADER);
	terrain_refl_shader.attach_file("terrain_refl_fs.glsl", GL_FRAGMENT_SHADER);
	terrain_refl_shader.link();

	terrain_refl_shader.set_uniform("shadow_distance", shadow_dist);
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
	terrain_refl_shader.set_uniform("cam_pos", camera.position);
	terrain_refl_shader.set_uniform("light_direction", light_direction);

	terrain_shadow_shader.attach_file("terrain_vs.glsl", GL_VERTEX_SHADER);
	terrain_shadow_shader.attach_file("terrain_tc.glsl", GL_TESS_CONTROL_SHADER);
	terrain_shadow_shader.attach_file("terrain_te.glsl", GL_TESS_EVALUATION_SHADER);
	terrain_shadow_shader.attach_file("terrain_shadow_fs.glsl", GL_FRAGMENT_SHADER);
	terrain_shadow_shader.link();

	terrain_shadow_shader.set_uniform_float("max_height", terrain_max_height);
	terrain_shadow_shader.set_uniform_int("tile_size", tile_size);
	terrain_shadow_shader.set_uniform_uint("terrain_side", terrain_side);
	terrain_shadow_shader.set_uniform_int("max_tess_level", App::sys_info.max_tess_level);
	terrain_shadow_shader.set_uniform("cam_pos", camera.position);

	display_shader.attach_file("display_vs.glsl", GL_VERTEX_SHADER);
	display_shader.attach_file("display_fs.glsl", GL_FRAGMENT_SHADER);
	display_shader.link();

	display_shader.set_uniform("cam_pos", camera.position);
	display_shader.set_uniform("light_direction", light_direction);

	water_shader.attach_file("water_vs.glsl", GL_VERTEX_SHADER);
	water_shader.attach_file("water_fs.glsl", GL_FRAGMENT_SHADER);
	water_shader.link();

	water_shader.set_uniform("shadow_distance", shadow_dist);
	water_shader.set_uniform("near_far", near_far);
	water_shader.set_uniform("cam_pos", camera.position);
	water_shader.set_uniform("light_direction", light_direction);
	water_shader.set_uniform_uint("terrain_side", terrain_side);

	skybox_shader.attach_file("skybox_vs.glsl", GL_VERTEX_SHADER);
	skybox_shader.attach_file("skybox_fs.glsl", GL_FRAGMENT_SHADER);
	skybox_shader.link();

	calculate_shadow_frustum();

	height_map.load("island.jpg");
	water_dudv.load("terrain_water_dudv.jpg");
	water_dudv.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	water_dudv.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	water_dudv.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	water_dudv.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	sand.load("terrain_sand.jpg");
	sand.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	sand.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	sand.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	sand.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	grass.load("terrain_grass.jpg");
	grass.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	grass.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	grass.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	grass.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	rock.load("terrain_rock.jpg");
	rock.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	rock.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	rock.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	rock.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	snow.load("terrain_snow.jpg");
	snow.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	snow.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	snow.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	snow.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	sky_tex.load("bluecloud_rt.jpg", "bluecloud_lf.jpg", "bluecloud_up.jpg",
		     "bluecloud_dn.jpg", "bluecloud_ft.jpg", "bluecloud_bk.jpg");

	depth_buffer.set_format(GL_DEPTH_COMPONENT);
	depth_buffer.set_size(width, height);

	color_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	shadow_tex.create_empty(width, height, GL_R16F, GL_FLOAT, GL_RED);
	position_tex.create_empty(width, height, GL_RGBA16F, GL_FLOAT, GL_RGBA);
	normal_tex.create_empty(width, height, GL_RGB16F, GL_FLOAT, GL_RGB);
	spec_tex.create_empty(width, height, GL_R16F, GL_FLOAT, GL_RED);
	fb_color.attach_texture(GL_COLOR_ATTACHMENT0, color_tex);
	fb_color.attach_texture(GL_COLOR_ATTACHMENT1, shadow_tex);
	fb_color.attach_texture(GL_COLOR_ATTACHMENT2, position_tex);
	fb_color.attach_texture(GL_COLOR_ATTACHMENT3, normal_tex);
	fb_color.attach_texture(GL_COLOR_ATTACHMENT4, spec_tex);
	fb_color.attach_renderbuffer(GL_DEPTH_ATTACHMENT, depth_buffer);
	fb_color.finalize();

	refraction_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	depth_tex.create_empty(width, height, GL_R32F, GL_FLOAT, GL_RED);
	fb_refract.attach_texture(GL_COLOR_ATTACHMENT0, refraction_tex);
	fb_refract.attach_texture(GL_COLOR_ATTACHMENT1, depth_tex);
	fb_refract.attach_renderbuffer(GL_DEPTH_ATTACHMENT, depth_buffer);
	fb_refract.finalize();

	reflection_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	fb_reflect.attach_texture(GL_COLOR_ATTACHMENT0, reflection_tex);
	fb_reflect.attach_renderbuffer(GL_DEPTH_ATTACHMENT, depth_buffer);
	fb_reflect.finalize();

	float tex_color[] = {1.0f, 0.0f, 0.0f, 1.0f};
	shadow_map[0].create_empty(4096, 4096, GL_DEPTH_COMPONENT32F, GL_FLOAT, GL_DEPTH_COMPONENT);
	shadow_map[0].set_parameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	shadow_map[0].set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	shadow_map[0].set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	shadow_map[0].set_parameter(GL_TEXTURE_BORDER_COLOR, tex_color);
	shadow_map[1].create_empty(4096, 4096, GL_DEPTH_COMPONENT32F, GL_FLOAT, GL_DEPTH_COMPONENT);
	shadow_map[1].set_parameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	shadow_map[1].set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	shadow_map[1].set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	shadow_map[1].set_parameter(GL_TEXTURE_BORDER_COLOR, tex_color);
	shadow_map[2].create_empty(4096, 4096, GL_DEPTH_COMPONENT32F, GL_FLOAT, GL_DEPTH_COMPONENT);
	shadow_map[2].set_parameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	shadow_map[2].set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	shadow_map[2].set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	shadow_map[2].set_parameter(GL_TEXTURE_BORDER_COLOR, tex_color);

	fb_shadow[0].attach_texture(GL_DEPTH_ATTACHMENT, shadow_map[0]);
	fb_shadow[0].finalize();
	fb_shadow[1].attach_texture(GL_DEPTH_ATTACHMENT, shadow_map[1]);
	fb_shadow[1].finalize();
	fb_shadow[2].attach_texture(GL_DEPTH_ATTACHMENT, shadow_map[2]);
	fb_shadow[2].finalize();

	display_mesh.model_matrix = glm::rotate((float)(M_PI / 2), glm::vec3(1, 0, 0));
	display_mesh.setup_shader(&display_shader);
	display_mesh.setup_camera(&camera);
	display_mesh.textures_misc.push_back(std::make_pair("color_texture", &color_tex));
	display_mesh.textures_misc.push_back(std::make_pair("normal_texture", &normal_tex));
	display_mesh.textures_misc.push_back(std::make_pair("position_texture", &position_tex));
	display_mesh.textures_misc.push_back(std::make_pair("spec_texture", &spec_tex));
	display_mesh.textures_misc.push_back(std::make_pair("shadow_texture", &shadow_tex));
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
	terrain_tile.textures_misc.push_back(std::make_pair("shadow_map_near", &shadow_map[0]));
	terrain_tile.textures_misc.push_back(std::make_pair("shadow_map_mid", &shadow_map[1]));
	terrain_tile.textures_misc.push_back(std::make_pair("shadow_map_far", &shadow_map[2]));

	float water_size = terrain_side * tile_size * 1.1f;
	water_mesh.model_matrix = glm::scale(glm::vec3(water_size, 1, water_size));
	water_mesh.model_matrix = glm::translate(glm::vec3(0, water_height, 0)) * water_mesh.model_matrix;
	water_mesh.setup_shader(&water_shader);
	water_mesh.setup_camera(&camera);
	water_mesh.add_vertex_attribute("pos_in", 4, GL_FLOAT, position);
	water_mesh.add_vertex_attribute("tc_in", 2, GL_FLOAT, tc);
	water_mesh.attach_index_buffer(ind);
	water_mesh.textures_misc.push_back(std::make_pair("water_dudv_texture", &water_dudv));
	water_mesh.textures_misc.push_back(std::make_pair("refraction_texture", &refraction_tex));
	water_mesh.textures_misc.push_back(std::make_pair("reflection_texture", &reflection_tex));
	water_mesh.textures_misc.push_back(std::make_pair("depth_texture", &depth_tex));
	water_mesh.textures_misc.push_back(std::make_pair("shadow_map_near", &shadow_map[0]));
	water_mesh.textures_misc.push_back(std::make_pair("shadow_map_mid", &shadow_map[1]));
	water_mesh.textures_misc.push_back(std::make_pair("shadow_map_far", &shadow_map[2]));

	skybox.model_matrix = glm::scale(glm::vec3(terrain_side * tile_size));
	skybox.setup_shader(&skybox_shader);
	skybox.setup_camera(&ip_cam);
	skybox.add_vertex_attribute("pos_in", 4, GL_FLOAT, skybox_pos);
	skybox.attach_index_buffer(skybox_ind);
	skybox.textures_misc.push_back(std::make_pair("sky_texture", &sky_tex));
}

Win::~Win() {
}

void Win::handle_resize() {
	glViewport(0, 0, width, height);
	depth_buffer.set_size(width, height);
	depth_tex.create_empty(width, height, GL_R32F, GL_FLOAT, GL_R);
	shadow_tex.create_empty(width, height, GL_R16F, GL_FLOAT, GL_R);
	color_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	normal_tex.create_empty(width, height, GL_RGB16F, GL_FLOAT, GL_RGB);
	position_tex.create_empty(width, height, GL_RGBA16F, GL_FLOAT, GL_RGBA);
	spec_tex.create_empty(width, height, GL_RGB16F, GL_FLOAT, GL_RGB);
	refraction_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	reflection_tex.create_empty(width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
	camera.width = static_cast<float>(width);
	camera.width = static_cast<float>(height);
	camera.update_projection_matrix();
	reflection_cam.width = static_cast<float>(width);
	reflection_cam.width = static_cast<float>(height);
	reflection_cam.update_projection_matrix();
	refl_ip_cam.width = static_cast<float>(width);
	refl_ip_cam.width = static_cast<float>(height);
	refl_ip_cam.update_projection_matrix();
}

void Win::calculate_shadow_frustum() {
	std::vector<P_Camera> cams(3);
	std::vector<glm::vec3> frustum_points(8);

	cams[0] = P_Camera(camera);
	cams[0].far_plane = shadow_dist[0];
	cams[0].update_projection_matrix();

	cams[1] = P_Camera(camera);
	cams[1].near_plane = shadow_dist[0];
	cams[1].far_plane = shadow_dist[1];
	cams[1].update_projection_matrix();

	cams[2] = P_Camera(camera);
	cams[2].near_plane = shadow_dist[1];
	cams[2].update_projection_matrix();

	for(unsigned int i = 0; i < cams.size(); i++) {
		shadow_cam[i].calculate_bounding_frustum(cams[i], light_direction, terrain_max_height * (i + 1));

		light_matrix[i] = shadow_cam[i].projection_matrix * shadow_cam[i].view_matrix;
	}

	terrain_shader.set_uniform("light_matrix", false, light_matrix);
	terrain_refr_shader.set_uniform("light_matrix", false, light_matrix);
	terrain_refl_shader.set_uniform("light_matrix", false, light_matrix);
	water_shader.set_uniform("light_matrix", false, light_matrix);
}

void Win::shadow_pass() {
	for(unsigned int i = 0; i < shadow_map.size(); i++) {
		fb_shadow[i].bind();
		glViewport(0, 0, shadow_map[i].width, shadow_map[i].height);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CLIP_DISTANCE0);

		terrain_tile.setup_camera(&shadow_cam[i]);
		terrain_tile.setup_shader(&terrain_shadow_shader);
		terrain_tile.draw_instanced(GL_PATCHES, tile_positions.size());
	}
}

void Win::reflect_pass() {
	glClearColor(1, 1, 1, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
	glDisable(GL_CULL_FACE);

	glDepthMask(GL_FALSE);
	skybox.setup_camera(&refl_ip_cam);
	skybox.draw(GL_TRIANGLES);
	glDepthMask(GL_TRUE);

	terrain_tile.setup_camera(&reflection_cam);
	terrain_tile.setup_shader(&terrain_refl_shader);
	terrain_tile.draw_instanced(GL_PATCHES, tile_positions.size());
}

void Win::refract_pass() {
	glClearColor(1, 1, 1, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_CULL_FACE);

	terrain_tile.setup_camera(&camera);
	terrain_tile.setup_shader(&terrain_refr_shader);
	terrain_tile.draw_instanced(GL_PATCHES, tile_positions.size());
}

void Win::color_pass() {
	glClearColor(1, 1, 1, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CLIP_DISTANCE0);
	glEnable(GL_CULL_FACE);

	glDepthMask(GL_FALSE);
	skybox.setup_camera(&ip_cam);
	skybox.draw(GL_TRIANGLES);
	glDepthMask(GL_TRUE);

	terrain_tile.setup_camera(&camera);
	terrain_tile.setup_shader(&terrain_shader);
	terrain_tile.draw_instanced(GL_PATCHES, tile_positions.size());

	water_shader.set_uniform_float("time", time);
	water_mesh.setup_camera(&camera);
	water_mesh.setup_shader(&water_shader);
	water_mesh.draw(GL_TRIANGLE_STRIP);
}

void Win::display() {
	time = timer.get_time_s();
	glClearColor(1, 1, 1, 1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if(wireframe) {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	skybox.model_matrix = glm::rotate(glm::radians(static_cast<float>(delta_time * 0.5)),
			      glm::vec3(0, 1, 0)) * skybox.model_matrix;

	shadow_pass();
	glViewport(0, 0, width, height);
	fb_reflect.bind();
	reflect_pass();
	fb_refract.bind();
	refract_pass();
	fb_color.bind();
	color_pass();
	fb_color.unbind();
	fb_color.blit_to(NULL, 0, 0, width, height, 0, 0,
			   width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
		skybox_shader.recompile();

		calculate_shadow_frustum();

		terrain_shader.set_uniform("shadow_distance", shadow_dist);
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
		terrain_shader.set_uniform("cam_pos", camera.position);
		terrain_shader.set_uniform("light_direction", light_direction);
		terrain_shader.set_uniform("clip_plane", glm::vec4(0, -1, 0, 0));

		terrain_refr_shader.set_uniform("shadow_distance", shadow_dist);
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
		terrain_refr_shader.set_uniform("cam_pos", camera.position);
		terrain_refr_shader.set_uniform("light_direction", light_direction);

		terrain_refl_shader.set_uniform("shadow_distance", shadow_dist);
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
		terrain_refl_shader.set_uniform("cam_pos", camera.position);
		terrain_refl_shader.set_uniform("light_direction", light_direction);

		terrain_shadow_shader.set_uniform_float("max_height", terrain_max_height);
		terrain_shadow_shader.set_uniform_int("tile_size", tile_size);
		terrain_shadow_shader.set_uniform_uint("terrain_side", terrain_side);
		terrain_shadow_shader.set_uniform_int("max_tess_level", App::sys_info.max_tess_level);
		terrain_shadow_shader.set_uniform("cam_pos", camera.position);

		display_shader.set_uniform("cam_pos", camera.position);
		display_shader.set_uniform("light_direction", light_direction);

		water_shader.set_uniform("shadow_distance", shadow_dist);
		water_shader.set_uniform("near_far", near_far);
		water_shader.set_uniform("cam_pos", camera.position);
		water_shader.set_uniform("light_direction", light_direction);
		water_shader.set_uniform_uint("terrain_side", terrain_side);
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
		ip_cam.move_right(mov_speed);
		update = true;
	} else if(key == "A") {
		camera.move_right(-mov_speed);
		ip_cam.move_right(-mov_speed);
		update = true;
	} else if(key == "W") {
		camera.move_forward(mov_speed);
		ip_cam.move_forward(mov_speed);
		update = true;
	} else if(key == "S") {
		camera.move_forward(-mov_speed);
		ip_cam.move_forward(-mov_speed);
		update = true;
	} else if(key == "R") {
		camera.move_up(mov_speed);
		ip_cam.move_up(mov_speed);
		update = true;
	} else if(key == "F") {
		camera.move_up(-mov_speed);
		ip_cam.move_up(-mov_speed);
		update = true;
	} else if(key == "E") {
		camera.roll(rot_speed);
		ip_cam.roll(rot_speed);
		reflection_cam.roll(-rot_speed);
		refl_ip_cam.roll(-rot_speed);
		update = true;
	} else if(key == "Q") {
		camera.roll(-rot_speed);
		ip_cam.roll(-rot_speed);
		reflection_cam.roll(rot_speed);
		refl_ip_cam.roll(rot_speed);
		update = true;
	} else if(key == "Z") {
		camera.yaw(rot_speed);
		ip_cam.yaw(rot_speed);
		reflection_cam.yaw(rot_speed);
		refl_ip_cam.yaw(rot_speed);
		update = true;
	} else if(key == "X") {
		camera.yaw(-rot_speed);
		ip_cam.yaw(-rot_speed);
		reflection_cam.yaw(-rot_speed);
		refl_ip_cam.yaw(-rot_speed);
		update = true;
	}
	if(update) {
		if(camera.position[1] <= 1.1 * water_height)
			camera.position[1] = 1.1 * water_height;
		reflection_cam.position = camera.position;
		reflection_cam.position[1] -= 2 * (reflection_cam.position[1] - water_height);
		refl_ip_cam.position = reflection_cam.position;
		camera.update_view_matrix();
		ip_cam.update_view_matrix();
		reflection_cam.update_view_matrix();
		refl_ip_cam.update_view_matrix();
		calculate_shadow_frustum();

		terrain_shader.set_uniform("cam_pos", camera.position);
		terrain_refr_shader.set_uniform("cam_pos", camera.position);
		terrain_refl_shader.set_uniform("cam_pos", camera.position);
		terrain_shadow_shader.set_uniform("cam_pos", camera.position);
		display_shader.set_uniform("cam_pos", camera.position);
		water_shader.set_uniform("cam_pos", camera.position);
	}
}

void Win::handle_mouse_motion(int x, int y) {
	if(rel_mode) {
		float rot_speed = (float)(2 * delta_time);
		camera.yaw(-glm::atan((float)x) * rot_speed);
		camera.pitch(-glm::atan((float)y) * rot_speed);
		camera.update_view_matrix();
		ip_cam.yaw(-glm::atan((float)x) * rot_speed);
		ip_cam.pitch(-glm::atan((float)y) * rot_speed);
		ip_cam.update_view_matrix();
		reflection_cam.yaw(-glm::atan((float)x) * rot_speed);
		reflection_cam.pitch(glm::atan((float)y) * rot_speed);
		reflection_cam.update_view_matrix();
		refl_ip_cam.yaw(-glm::atan((float)x) * rot_speed);
		refl_ip_cam.pitch(glm::atan((float)y) * rot_speed);
		refl_ip_cam.update_view_matrix();
		calculate_shadow_frustum();
	}
}

int main(int argc, char **argv) {
	App::init();
	App::chdir_to_bin(argv);

	Shader::add_path("../island/shaders");
	Texture::add_path("../data/textures");

	int w = (int)(0.75 * App::sys_info.display_bounds[App::sys_info.num_displays - 1].w);
	int h = (int)(0.75 * App::sys_info.display_bounds[App::sys_info.num_displays - 1].h);
	int x = App::sys_info.display_bounds[App::sys_info.num_displays - 1].x +
		(int)(0.125 * App::sys_info.display_bounds[App::sys_info.num_displays - 1].w);
	int y = App::sys_info.display_bounds[App::sys_info.num_displays - 1].y +
		(int)(0.125 * App::sys_info.display_bounds[App::sys_info.num_displays - 1].h);

	Win window("Island", w, h, x, y);
	if(window.gl_maj < 4 && window.gl_min < 3)
		return -1;

	window.run(100);

	return 0;
}
