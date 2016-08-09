#ifndef __GUI_H__
#define __GUI_H__

#include <sgltk/window.h>
#include <sgltk/mesh.h>
#include <sgltk/scene.h>
#include <sgltk/camera.h>
#include <sgltk/texture.h>
#include <sgltk/timer.h>
#include <sgltk/shader.h>

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

	Shader *material_shader;
	Shader *textured_shader;
	Shader *floor_shader;
	Shader *fps_shader;
	Shader *point_shader;

	Texture *tex;
	Texture *floor_diff;
	Texture *floor_spec;
	Texture *floor_norm;
	Texture *floor_displ;
	Texture *floor_light;

	Mesh *fps_display;
	Mesh *floor;
	Mesh *earth;

	Scene *material_model;
	Scene *textured_model;

	Camera *fps_camera;
	Camera *camera;

	sgltk::Timer time;

	glm::vec3 light_pos;
	std::vector<sgltk::Vertex> light_verts;
	sgltk::Mesh *light;
	double time_cnt;
	unsigned int fps;
	unsigned int frame_cnt;

	Mesh *create_sphere(unsigned int slices, unsigned int stacks);
public:
	GUI(const char *title, int res_x, int res_y, int offset_x,
	    int offset_y, int gl_maj, int gl_min, unsigned int flags);

	~GUI();

	void handle_resize();
	void handle_keyboard(std::string key);
	void handle_key_press(std::string key, bool pressed);
	void handle_mouse_motion(int x, int y);
	void handle_mouse_wheel(int x, int y);
	void handle_mouse_button(int x, int y, int button, bool state, int clicks);
	void display();
};

#endif

