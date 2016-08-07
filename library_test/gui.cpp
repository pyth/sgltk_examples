#include "gui.h"

#define NUM_TILES 400

GUI::GUI(const char *title, int res_x, int res_y, int offset_x,
	 int offset_y, int gl_maj, int gl_min, unsigned int flags)
	 :Window(title, res_x, res_y, offset_x, offset_y, gl_maj, gl_min, flags) {

	//enable textures and blending
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLint max_patch_vertices = 0;
	GLint max_tess_gen_level = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &max_patch_vertices);
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &max_tess_gen_level);
	//printf("Max supported patch vertices %d\n", max_patch_vertices);
	//printf("Max tessellation level %d\n", max_tess_gen_level);
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	//Face culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT, GL_FILL);

	time_cnt = 0;

	floor_diff = new Texture("tile_sandstone_d.png");
	floor_diff->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_diff->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	floor_spec = new Texture("tile_sandstone_s.png");
	floor_spec->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_spec->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	floor_norm = new Texture("tile_sandstone_n.png");
	floor_norm->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_norm->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	floor_displ = new Texture("tile_sandstone_h.png");
	floor_displ->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_displ->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	floor_light = new Texture("tile_sandstone_a.png");
	floor_light->set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_light->set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//set up the cameras
	fps_camera = new Camera(sgltk::ORTHOGRAPHIC);
	camera = new Camera(glm::vec3(0,5,20), glm::vec3(0,0,-1),
			    glm::vec3(0,1,0), 70.0f, res_x,
			    res_y, 0.1f, 800.0f);

	//load the shaders
	material_shader = new Shader();
	material_shader->attach_file("material_vs.glsl", GL_VERTEX_SHADER);
	material_shader->attach_file("material_fs.glsl", GL_FRAGMENT_SHADER);
	material_shader->link();

	textured_shader = new Shader();
	textured_shader->attach_file("textured_vs.glsl", GL_VERTEX_SHADER);
	textured_shader->attach_file("textured_fs.glsl", GL_FRAGMENT_SHADER);
	textured_shader->link();

	floor_shader = new Shader();
	floor_shader->attach_file("floor_vs.glsl", GL_VERTEX_SHADER);
	floor_shader->attach_file("floor_tc.glsl", GL_TESS_CONTROL_SHADER);
	floor_shader->attach_file("floor_te.glsl", GL_TESS_EVALUATION_SHADER);
	floor_shader->attach_file("floor_fs.glsl", GL_FRAGMENT_SHADER);
	floor_shader->link();

	fps_shader = new Shader();
	fps_shader->attach_file("fps_vs.glsl", GL_VERTEX_SHADER);
	fps_shader->attach_file("fps_fs.glsl", GL_FRAGMENT_SHADER);
	fps_shader->link();

	point_shader = new Shader();
	point_shader->attach_file("point_shader_vs.glsl", GL_VERTEX_SHADER);
	point_shader->attach_file("point_shader_fs.glsl", GL_FRAGMENT_SHADER);
	point_shader->link();

	//arrange a few vertices into a square
	std::vector<sgltk::Vertex> vert;
	vert.push_back(sgltk::Vertex(glm::vec3(-2,-2,0.0),
				     glm::vec3(0.0,0.0,1.0),
				     glm::vec3(1.0,0.0,0.0),
				     glm::vec3(0.0,1.0,0.0)));
	vert.push_back(sgltk::Vertex(glm::vec3(2,-2,0.0),
				     glm::vec3(0.0,0.0,1.0),
				     glm::vec3(1.0,0.0,0.0),
				     glm::vec3(1.0,1.0,0.0)));
	vert.push_back(sgltk::Vertex(glm::vec3(-2,2,0.0),
				     glm::vec3(0.0,0.0,1.0),
				     glm::vec3(1.0,0.0,0.0),
				     glm::vec3(0.0,0.0,0.0)));
	vert.push_back(sgltk::Vertex(glm::vec3(2,2,0.0),
				     glm::vec3(0.0,0.0,1.0),
				     glm::vec3(1.0,0.0,0.0),
				     glm::vec3(1.0,0.0,0.0)));

	std::vector<unsigned short> floor_ind = {0, 1, 2, 3};
	std::vector<unsigned short> fps_ind = {0, 1, 2, 1, 3, 2};

	//create a mesh out of the vertices
	fps_display = new Mesh();
	fps_display->attach_vertex_buffer(&vert);
	fps_display->attach_index_buffer(&fps_ind);
	fps_display->setup_shader(fps_shader);
	fps_display->setup_camera(&fps_camera->view_matrix,
			   &fps_camera->projection_matrix_ortho);
	fps_display->set_vertex_attribute("pos_in", 0, 4, GL_FLOAT, sizeof(sgltk::Vertex),
				   (void*)offsetof(sgltk::Vertex, position));
	fps_display->set_vertex_attribute("tex_coord_in0", 0, 3, GL_FLOAT, sizeof(sgltk::Vertex),
				   (void*)offsetof(sgltk::Vertex, tex_coord));
	fps_display->model_matrix = glm::scale(glm::vec3(12.0, 7.0, 0.0));

	floor = new Mesh();
	floor->attach_vertex_buffer(&vert);
	floor->attach_index_buffer(&floor_ind);
	floor->setup_shader(floor_shader);
	floor->setup_camera(&camera->view_matrix,
			    &camera->projection_matrix_persp);
	floor->set_vertex_attribute("pos_in", 0, 4, GL_FLOAT, sizeof(sgltk::Vertex),
				   (void*)offsetof(sgltk::Vertex, position));
	floor->set_vertex_attribute("norm_in", 0, 3, GL_FLOAT, sizeof(sgltk::Vertex),
				   (void*)offsetof(sgltk::Vertex, normal));
	floor->set_vertex_attribute("tang_in", 0, 4, GL_FLOAT, sizeof(sgltk::Vertex),
				   (void*)offsetof(sgltk::Vertex, tangent));
	floor->set_vertex_attribute("tex_coord_in0", 0, 3, GL_FLOAT, sizeof(sgltk::Vertex),
				   (void*)offsetof(sgltk::Vertex, tex_coord));
	std::vector<glm::mat4> floor_m(NUM_TILES);
	std::vector<glm::mat3> floor_nm(NUM_TILES);
	//glm::mat4 floor_scale = glm::scale(glm::vec3(2.0, 0.0, 2.0));
	glm::mat4 floor_rot = glm::rotate((float)(-M_PI / 2), glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 floor_transl;
	int side = sqrt(floor_m.size());
	for(int i = -side / 2; i < side / 2; i++) {
		for(int j = -side / 2; j < side / 2; j++) {
			int index = (i + side / 2) * side + j + side / 2;
			//std::cout<<index<<" "<<i<<" "<<j<<"\n\n";
			floor_transl = glm::translate(glm::vec3(-40, 0, -40));
			floor_m[index] = floor_transl * floor_rot;
			/*for(int k=0;k<4;k++) {
				for(int l=0;l<4;l++)
					std::cout<<floor_m[index][l][k]<<" ";
				std::cout<<"\n";
			}
			std::cout<<"\n";*/
			floor_nm[index] = glm::mat3(glm::transpose(glm::inverse(floor_m[index])));
		}
	}
	int loc, buf_ind;
	unsigned int vec3_size = sizeof(glm::vec3);
	unsigned int vec4_size = sizeof(glm::vec4);
	loc = glGetAttribLocation(floor_shader->program, "model_matrix");
	buf_ind = floor->attach_vertex_buffer<glm::mat4>(&floor_m);
	for(int i = 0; i < 4; i++) {
		floor->set_vertex_attribute(loc + i, buf_ind, 4, GL_FLOAT, sizeof(glm::mat4), (const void*)(i * vec4_size));
		floor->set_vertex_attribute_divisor(loc + i, 1);
	}

	loc = glGetAttribLocation(floor_shader->program, "normal_matrix");
	buf_ind = floor->attach_vertex_buffer<glm::mat3>(&floor_nm);
	for(int i = 0; i < 3; i++) {
		floor->set_vertex_attribute(loc + i, buf_ind, 3, GL_FLOAT, sizeof(glm::mat3), (const void*)(i * vec3_size));
		floor->set_vertex_attribute_divisor(loc + i, 1);
	}

	floor->textures_diffuse = {floor_diff};
	floor->textures_normals = {floor_norm};
	floor->textures_specular = {floor_spec};
	floor->textures_lightmap = {floor_light};
	floor->textures_displacement = {floor_displ};

	material_model = new Scene();
	material_model->setup_shader(material_shader);
	material_model->setup_camera(&camera->view_matrix, &camera->projection_matrix_persp);
	material_model->load("Spikey.dae");
	material_model->set_animation_speed(1.0);

	textured_model = new Scene();
	textured_model->setup_shader(textured_shader);
	textured_model->setup_camera(&camera->view_matrix, &camera->projection_matrix_persp);
	textured_model->load("bob_lamp.md5mesh");
	textured_model->set_animation_speed(1.0);

	time = sgltk::Timer();

	light_pos = glm::vec3(0, 10, 0);
	light_verts.push_back(sgltk::Vertex(glm::vec3(0.0, 0.0, 0.0),
					  glm::vec3(0.0),
					  glm::vec3(0.0),
					  glm::vec4(1.0, 0.0, 0.0, 0.0),
					  glm::vec3(0.0)));
	std::vector<unsigned short> light_index = {0};
	light_trafo = glm::mat4(1.0);
	light_trafo = glm::translate(light_trafo, light_pos);
	light = new sgltk::Mesh();
	light->attach_vertex_buffer(&light_verts);
	light->attach_index_buffer(&light_index);
	light->setup_shader(point_shader);
	light->setup_camera(&camera->view_matrix, &camera->projection_matrix_persp);
	light->set_vertex_attribute("pos_in", 0, 3, GL_FLOAT, sizeof(sgltk::Vertex),
					(void *)offsetof(sgltk::Vertex, position));
	light->set_vertex_attribute("color_in", 0, 4, GL_FLOAT, sizeof(sgltk::Vertex),
					(void *)offsetof(sgltk::Vertex, color));

	for(int i = 0; i < 1; i++) {
		for(int j = 0; j < 1; j++) {
			spikey_trafos.push_back(glm::translate(glm::vec3((float)i*4, -0.1f, (float)j*4)));
		}
	}

	for(int i = -1; i < 0; i++) {
		for(int j = 0; j < 1; j++) {
			bob_trafos.push_back(glm::translate(glm::vec3((float)i*4, 0.0f, (float)j*4)));
		}
	}

	fps = 0;
	wireframe = false;
	wireframe_change = false;
	rel_mode = true;
	mouse_mode_change = false;
	set_relative_mode(true);
}

GUI::~GUI() {
	delete tex;
	delete floor_diff;
	delete floor_spec;
	delete floor_norm;
	delete floor_displ;
	delete floor_light;

	delete fps_camera;
	delete camera;

	delete material_shader;
	delete textured_shader;
	delete floor_shader;
	delete fps_shader;
	delete point_shader;

	delete floor;
	delete fps_display;
	delete light;
	delete material_model;
	delete textured_model;
}

Mesh *GUI::create_sphere(unsigned int slices, unsigned int stacks) {
	Mesh *tmp = new Mesh();
	return tmp;
}

void GUI::display() {
	glClearColor(0,0,0,1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	time_cnt += delta_time;
	frame_cnt++;
	if(time_cnt >= 1.0) {
		fps = frame_cnt;
		frame_cnt = 0;
		time_cnt -= 1.0;
	}
	Image fps_text;
	Texture fps_tex;
	fps_text.create_text("FPS: " + std::to_string(fps),
			     "Oswald-Medium.ttf", 40, 255, 0, 0, 255);
	fps_tex.load_texture(&fps_text);

	fps_shader->bind();
	int texture_loc = glGetUniformLocation(fps_shader->program,
						    "Texture");
	glUniform1i(texture_loc, 0);
	int res_loc = glGetUniformLocation(fps_shader->program,
						    "Resolution");
	glUniform2f(res_loc, fps_camera->width, fps_camera->height);

	material_shader->bind();
	int light_loc = glGetUniformLocation(material_shader->program,
					  "light_pos");
	glUniform3fv(light_loc, 1, glm::value_ptr(light_pos));
	textured_shader->bind();
	light_loc = glGetUniformLocation(textured_shader->program,
					  "light_pos");
	glUniform3fv(light_loc, 1, glm::value_ptr(light_pos));
	int cam_loc = glGetUniformLocation(textured_shader->program,
					  "cam_pos");
	glUniform3fv(cam_loc, 1, glm::value_ptr(camera->pos));

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	fps_tex.bind();
	fps_display->draw(GL_TRIANGLES);

	if(wireframe) {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	floor->draw_instanced(GL_PATCHES, NUM_TILES);

	for(unsigned int i = 0; i < spikey_trafos.size(); i++) {
		material_model->animate(time.get_time());
		material_model->draw(&spikey_trafos[i]);
	}

	for(unsigned int i = 0; i < bob_trafos.size(); i++) {
		textured_model->animate(time.get_time());
		textured_model->draw(&bob_trafos[i]);
	}

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPointSize(10);
	light->draw(GL_POINTS, &light_trafo);
}

void GUI::handle_resize() {
	glViewport(0, 0, width, height);
	camera->update_projection_matrix(width, height);
}

void GUI::handle_key_press(std::string key, bool pressed) {
	if(key == "Escape") {
		stop();
	} else if(key == "M") {
		if(pressed) {
			rel_mode = !rel_mode;
			set_relative_mode(rel_mode);
		}
	} else if(key == "P") {
		if(pressed) {
			material_shader->recompile();
			textured_shader->recompile();
			floor_shader->recompile();
			point_shader->recompile();
			fps_shader->recompile();
		}
	} else if(key == "L") {
		if(pressed) {
			wireframe = !wireframe;
		}
	}
}

void GUI::handle_keyboard(std::string key) {
	float mov_speed = 0.1;
	float rot_speed = 0.01;
	float dt = 1000 * delta_time;
	if(dt < 2.0)
		dt = 2.0;

	if(key == "D") {
		camera->move_right(mov_speed * dt);
	} else if(key == "A") {
		camera->move_right(-mov_speed * dt);
	}else if(key == "W") {
		camera->move_forward(mov_speed * dt);
	} else if(key == "S") {
		camera->move_forward(-mov_speed * dt);
	} else if(key == "R") {
		camera->move_up(mov_speed * dt);
	} else if(key == "F") {
		camera->move_up(-mov_speed * dt);
	} else if(key == "E") {
		camera->roll(rot_speed * dt);
	} else if(key == "Q") {
		camera->roll(-rot_speed * dt);
	}
	camera->update_view_matrix();
}

void GUI::handle_mouse_motion(int x, int y) {
	if(rel_mode) {
		camera->yaw(-glm::atan((float)x)/500);
		camera->pitch(-glm::atan((float)y)/500);
		camera->update_view_matrix();
	}
}

void GUI::handle_mouse_wheel(int x, int y) {
}

void GUI::handle_mouse_button(int x, int y, int button, bool state, int clicks) {
	printf("%i %i\t%i %i %i\n", x, y, button, state, clicks);
	fflush(stdout);
}
