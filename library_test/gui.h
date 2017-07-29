#ifndef __GUI_H__
#define __GUI_H__

#include <sgltk/sgltk.h>

using namespace sgltk;

typedef struct point_light {
	glm::vec3 position;
	glm::vec4 color;
	float linear;
	float quadratic;
} point_light;

class GUI : public Window {
	bool windowed;
	bool wireframe;
	bool rel_mode;
	bool ctrl;

	glm::mat4 fps_trafo;
	glm::mat4 light_trafo;
	std::vector<glm::mat4> spikey_trafos;
	std::vector<glm::mat4> bob_trafos;

	Shader material_shader;
	Shader textured_shader;
	Shader floor_shader;
	Shader fps_shader;
	Shader point_shader;

	Texture_2d fps_tex;
	Texture_2d floor_diff;
	Texture_2d floor_spec;
	Texture_2d floor_norm;
	Texture_2d floor_displ;
	Texture_2d floor_light;

	Mesh fps_display;
	Mesh floor;
	Mesh light;

	Model material_model;
	Model textured_model;

	P_Camera camera;

	sgltk::Timer time;

	glm::vec3 light_pos;
	std::vector<sgltk::Vertex> light_verts;
	double frame_sum;
	unsigned int fps;
	unsigned int frame_cnt;
#ifdef HAVE_SDL_TTF_H
	TTF_Font *font;
#endif //HAVE_SDL_TTF_H

	Mesh *create_sphere(unsigned int slices, unsigned int stacks);

	void handle_resize();
	void handle_gamepad_button_press(unsigned int id, int button, bool pressed);
	void handle_gamepad_button(unsigned int id, int button);
	void handle_gamepad_axis(unsigned int id, unsigned int axis, int value);
	void handle_keyboard(const std::string& key);
	void handle_key_press(const std::string& key, bool pressed);
	void handle_mouse_motion(int x, int y);
	void handle_mouse_wheel(int x, int y);
	void handle_mouse_button(int x, int y, int button, bool state, int clicks);
	void display();
public:
	GUI(const std::string& title, int res_x, int res_y, int offset_x, int offset_y);

	~GUI();
};

#endif
