#include "gui.h"

#define NUM_TILES 400

GUI::GUI(const std::string& title, int res_x, int res_y, int offset_x, int offset_y)
	 :Window(title, res_x, res_y, offset_x, offset_y) {

	sgltk::App::enable_vsync(true);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	//Face culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

#ifdef HAVE_SDL_TTF_H
	font = Image::open_font_file("Oswald-Medium.ttf", 40);

	Image fps_text;
	fps_text.create_text("FPS: " + std::to_string(0),
			     font, 255, 0, 0, 255);
	fps_tex.load(fps_text);
#endif //HAVE_SDL_TTF_H

	floor_diff.load("tile_sandstone_d.png");
	floor_diff.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_diff.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	floor_spec.load("tile_sandstone_s.png");
	floor_spec.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_spec.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	floor_norm.load("tile_sandstone_n.png");
	floor_norm.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_norm.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	floor_displ.load("tile_sandstone_h.png");
	floor_displ.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_displ.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	floor_light.load("tile_sandstone_a.png");
	floor_light.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	floor_light.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//set up the cameras
	camera = P_Camera(glm::vec3(0,5,20), glm::vec3(0,0,-1),
			  glm::vec3(0,1,0), glm::radians(70.0f),
			  (float)res_x, (float)res_y, 0.1f, 800.0f);

	//load the shaders
	material_shader.attach_file("material_vs.glsl", GL_VERTEX_SHADER);
	material_shader.attach_file("material_fs.glsl", GL_FRAGMENT_SHADER);
	material_shader.link();

	textured_shader.attach_file("textured_vs.glsl", GL_VERTEX_SHADER);
	textured_shader.attach_file("textured_fs.glsl", GL_FRAGMENT_SHADER);
	textured_shader.link();

	floor_shader.attach_file("floor_vs.glsl", GL_VERTEX_SHADER);
	floor_shader.attach_file("floor_tc.glsl", GL_TESS_CONTROL_SHADER);
	floor_shader.attach_file("floor_te.glsl", GL_TESS_EVALUATION_SHADER);
	floor_shader.attach_file("floor_fs.glsl", GL_FRAGMENT_SHADER);
	floor_shader.link();

#ifdef HAVE_SDL_TTF_H
	fps_shader.attach_file("fps_vs.glsl", GL_VERTEX_SHADER);
	fps_shader.attach_file("fps_fs.glsl", GL_FRAGMENT_SHADER);
	fps_shader.link();
#endif

	point_shader.attach_file("point_shader_vs.glsl", GL_VERTEX_SHADER);
	point_shader.attach_file("point_shader_fs.glsl", GL_FRAGMENT_SHADER);
	point_shader.link();

	//arrange a few vertices into a square
	std::vector<sgltk::Vertex> vert;
	vert.push_back(sgltk::Vertex(glm::vec3(-0.5, -0.5, 0.0),
				     glm::vec3( 0.0,  0.0, 1.0),
				     glm::vec3( 1.0,  0.0, 0.0),
				     glm::vec3( 0.0,  1.0, 0.0)));
	vert.push_back(sgltk::Vertex(glm::vec3( 0.5, -0.5, 0.0),
				     glm::vec3( 0.0,  0.0, 1.0),
				     glm::vec3( 1.0,  0.0, 0.0),
				     glm::vec3( 1.0,  1.0, 0.0)));
	vert.push_back(sgltk::Vertex(glm::vec3(-0.5,  0.5, 0.0),
				     glm::vec3( 0.0,  0.0, 1.0),
				     glm::vec3( 1.0,  0.0, 0.0),
				     glm::vec3( 0.0,  0.0, 0.0)));
	vert.push_back(sgltk::Vertex(glm::vec3( 0.5,  0.5, 0.0),
				     glm::vec3( 0.0,  0.0, 1.0),
				     glm::vec3( 1.0,  0.0, 0.0),
				     glm::vec3( 1.0,  0.0, 0.0)));

	std::vector<unsigned short> floor_ind = {0, 1, 2, 3};
	std::vector<unsigned short> fps_ind = {0, 1, 2, 1, 3, 2};

	//create a mesh out of the vertices
	fps_display.attach_vertex_buffer(vert);
	fps_display.attach_index_buffer(fps_ind);
	fps_display.setup_shader(&fps_shader);
	fps_display.set_vertex_attribute("pos_in", 0, 4, GL_FLOAT, sizeof(sgltk::Vertex),
					 (void*)offsetof(sgltk::Vertex, position));
	fps_display.set_vertex_attribute("tex_coord_in0", 0, 3, GL_FLOAT, sizeof(sgltk::Vertex),
					 (void*)offsetof(sgltk::Vertex, tex_coord));

	floor.attach_vertex_buffer(vert);
	floor.attach_index_buffer(floor_ind);
	floor.setup_shader(&floor_shader);
	floor.setup_camera(&camera);
	floor.set_vertex_attribute("pos_in", 0, 4, GL_FLOAT, sizeof(sgltk::Vertex),
				   (void*)offsetof(sgltk::Vertex, position));
	floor.set_vertex_attribute("norm_in", 0, 3, GL_FLOAT, sizeof(sgltk::Vertex),
				   (void*)offsetof(sgltk::Vertex, normal));
	floor.set_vertex_attribute("tang_in", 0, 4, GL_FLOAT, sizeof(sgltk::Vertex),
				   (void*)offsetof(sgltk::Vertex, tangent));
	floor.set_vertex_attribute("tex_coord_in0", 0, 3, GL_FLOAT, sizeof(sgltk::Vertex),
				   (void*)offsetof(sgltk::Vertex, tex_coord));
	std::vector<glm::mat4> floor_m(NUM_TILES);
	std::vector<glm::mat3> floor_nm(NUM_TILES);
	glm::mat4 floor_scale = glm::scale(glm::vec3(4, 4, 1));
	glm::mat4 floor_rot = glm::rotate((float)(-M_PI / 2), glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 floor_transl;
	int side = (int)sqrt(floor_m.size());
	for(int i = -side / 2; i < side / 2; i+=1) {
		for(int j = -side / 2; j < side / 2; j+=1) {
			int index = (i + side / 2) * side + j + side / 2;
			floor_transl = glm::translate(glm::vec3(4*i, 0, 4*j));
			floor_m[index] = floor_transl * floor_rot * floor_scale;
			floor_nm[index] = glm::mat3(glm::transpose(glm::inverse(floor_m[index])));
		}
	}
	int loc, buf_ind;
	unsigned int vec3_size = sizeof(glm::vec3);
	unsigned int vec4_size = sizeof(glm::vec4);
	loc = floor_shader.get_attribute_location("model_matrix");
	buf_ind = floor.attach_vertex_buffer<glm::mat4>(floor_m);
	for(int i = 0; i < 4; i++) {
		floor.set_vertex_attribute(loc + i, buf_ind, 4, GL_FLOAT,
						sizeof(glm::mat4),
						reinterpret_cast<const void*>(i * vec4_size), 1);
	}

	loc = floor_shader.get_attribute_location("normal_matrix");
	buf_ind = floor.attach_vertex_buffer<glm::mat3>(floor_nm);
	for(int i = 0; i < 3; i++) {
		floor.set_vertex_attribute(loc + i, buf_ind, 3, GL_FLOAT,
						sizeof(glm::mat3),
						reinterpret_cast<const void*>(i * vec3_size), 1);
	}

	floor.attach_texture("texture_diffuse", floor_diff);
	floor.attach_texture("texture_normals", floor_norm);
	floor.attach_texture("texture_specular", floor_spec);
	floor.attach_texture("texture_lightmap", floor_light);
	floor.attach_texture("texture_displacement", floor_displ);

	material_model.setup_shader(&material_shader);
	material_model.setup_camera(&camera);
	material_model.load("Spikey.dae");
	material_model.set_animation_speed(1.0);

	textured_model.setup_shader(&textured_shader);
	textured_model.setup_camera(&camera);
	textured_model.load("bob_lamp.md5mesh");
	textured_model.set_animation_speed(1.0);

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
	light.attach_vertex_buffer(light_verts);
	light.attach_index_buffer(light_index);
	light.setup_shader(&point_shader);
	light.setup_camera(&camera);
	light.set_vertex_attribute("pos_in", 0, 3, GL_FLOAT, sizeof(sgltk::Vertex),
					(void *)offsetof(sgltk::Vertex, position));
	light.set_vertex_attribute("color_in", 0, 4, GL_FLOAT, sizeof(sgltk::Vertex),
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
	windowed = true;
	rel_mode = true;
	ctrl = false;
	set_relative_mode(true);
}

GUI::~GUI() {
#ifdef HAVE_SDL_TTF_H
	Image::close_font_file(font);
#endif
}

Mesh *GUI::create_sphere(unsigned int slices, unsigned int stacks) {
	Mesh *tmp = new Mesh();
	return tmp;
}

void GUI::display() {
	glClearColor(0,0,0,1);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

#ifdef HAVE_SDL_TTF_H
	frame_cnt++;
	frame_sum += 1.0 / delta_time;
	if(frame_cnt > 100) {
		fps = (unsigned int)(frame_sum / frame_cnt);
		Image fps_text;
		fps_text.create_text("FPS: " + std::to_string(fps),
				     font, 255, 0, 0, 255);
		fps_tex.load(fps_text);
		frame_cnt = 0;
		frame_sum = 0;
	}
	set_title("Test "+std::to_string(fps));
#endif //HAVE_SDL_TTF_H

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);

	fps_shader.set_uniform_int("Texture", 0);
	fps_tex.bind();
	fps_display.draw(GL_TRIANGLES);

	if(wireframe) {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	floor_shader.set_uniform("cam_pos", camera.position);
	floor_shader.set_uniform("light_pos", light_pos);
	floor_shader.set_uniform_int("max_tess_level", App::sys_info.max_tess_level);
	floor.draw_instanced(GL_PATCHES, NUM_TILES);

	material_shader.set_uniform("light_pos", light_pos);
	for(unsigned int i = 0; i < spikey_trafos.size(); i++) {
		material_model.animate((float)time.get_time_s());
		material_model.draw(&spikey_trafos[i]);
	}

	textured_shader.set_uniform("light_pos", light_pos);
	textured_shader.set_uniform("cam_pos", camera.position);
	for(unsigned int i = 0; i < bob_trafos.size(); i++) {
		textured_model.animate((float)time.get_time_s());
		textured_model.draw(&bob_trafos[i]);
	}

	glPointSize(10);
	light.draw(GL_POINTS, &light_trafo);
}

void GUI::handle_resize() {
	glViewport(0, 0, width, height);
	camera.width = static_cast<float>(width);
	camera.height = static_cast<float>(height);
	camera.update_projection_matrix();
}

void GUI::handle_gamepad_added(std::shared_ptr<Gamepad> gamepad) {
	gamepad->set_deadzone(1600);
}

void GUI::handle_gamepad_button_press(std::shared_ptr<Gamepad> gamepad, int button, bool pressed) {
	if(gamepad->id != 0)
		return;

	if(pressed) {
		switch(button) {
			case 0:
				wireframe = !wireframe;
				break;
		}
	}
}

void GUI::handle_gamepad_button(std::shared_ptr<Gamepad> gamepad, int button) {
	if(gamepad->id != 0)
		return;

	switch(button) {
		case 9: //L1
			camera.roll(-static_cast<float>(delta_time));
			camera.update_view_matrix();
			break;
		case 10: //R1
			camera.roll(static_cast<float>(delta_time));
			camera.update_view_matrix();
			break;
	}
}

void GUI::handle_gamepad_axis(std::shared_ptr<Gamepad> gamepad, unsigned int axis, int value) {
	if(gamepad->id != 0)
		return;

	switch(axis) {
		case 0: //left stick x-axis
			camera.move_right(value / 1000 * static_cast<float>(delta_time));
			break;
		case 1: //left stick y-axis
			camera.move_forward(-value / 1000 * static_cast<float>(delta_time));
			break;
		case 2: //right stick x-axis
			camera.yaw(-value / 10000 * static_cast<float>(delta_time));
			break;
		case 3: //right stick y-axis
			camera.pitch(-value / 10000 * static_cast<float>(delta_time));
			break;
		case 4: //L2
			camera.move_up(-value / 1000 * static_cast<float>(delta_time));
			break;
		case 5: //R2
			camera.move_up(value / 1000 * static_cast<float>(delta_time));
			break;
	}
	camera.update_view_matrix();
}

void GUI::handle_key_press(const std::string& key, bool pressed) {
	SDL_DisplayMode mode;
	if(key == "Escape") {
		stop();
	} else if(key == "M") {
		if(pressed) {
			rel_mode = !rel_mode;
			set_relative_mode(rel_mode);
		}
	} else if(key == "P") {
		if(pressed) {
			material_shader.recompile();
			textured_shader.recompile();
			floor_shader.recompile();
			point_shader.recompile();
			fps_shader.recompile();
		}
	} else if(key == "L") {
		if(pressed) {
			wireframe = !wireframe;
		}
	} else if(key == "Left Ctrl") {
		if(pressed)
			ctrl = true;
		else
			ctrl = false;
	} else if(key == "F") {
		if(pressed && ctrl) {
			if(windowed) {
				mode = App::sys_info.desktop_display_modes[get_display_index()];
				set_display_mode(mode);
				fullscreen_mode(WINDOW_MODE::FULLSCREEN);
			} else {
				fullscreen_mode(WINDOW_MODE::WINDOWED);
			}
			windowed = !windowed;
		}
	}
}

void GUI::handle_keyboard(const std::string& key) {
	bool update = false;
	float mov_speed = 50.0f;
	float rot_speed = 2.0f;

	if(key == "D") {
		camera.move_right(mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "A") {
		camera.move_right(-mov_speed * static_cast<float>(delta_time));
		update = true;
	}else if(key == "W") {
		camera.move_forward(mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "S") {
		camera.move_forward(-mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "R") {
		camera.move_up(mov_speed * static_cast<float>(delta_time));
		update = true;
	} else if(key == "F") {
		if(!ctrl) {
			camera.move_up(-mov_speed * static_cast<float>(delta_time));
			update = true;
		}
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

void GUI::handle_mouse_motion(int x, int y) {
	if(rel_mode) {
		camera.yaw(-glm::atan((float)x) * static_cast<float>(delta_time));
		camera.pitch(-glm::atan((float)y) * static_cast<float>(delta_time));
		camera.update_view_matrix();
	}
}

void GUI::handle_mouse_wheel(int x, int y) {
}

void GUI::handle_mouse_button(int x, int y, int button, bool state, int clicks) {
	printf("%i %i\t%i %i %i\n", x, y, button, state, clicks);
	fflush(stdout);
}
