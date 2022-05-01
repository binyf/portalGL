#include "cgmath.h"		// slee's simple math library
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "cgut.h"		// slee's OpenGL utility
#include "Camera.h"
#include "ViewProjectionMatrix.h"
#include "DragHistory.h"
#include "Light.h"
#include "Material.h"
#include "BindedVertexInfo.h"
#include "BindedTextureInfo.h"
#include "TextureBinder.h"
#include "Renderer.h"
#include "GameObject.h"
#include "GameAimingObject.h"
#include "GameMovingObject.h"
#include "SphereRenderer.h"
#include "BlockRenderer.h"
#include "BlockVerticesBinder.h"
#include "PortalXVerticesBinder.h"
#include "PortalYVerticesBinder.h"
#include "PortalZVerticesBinder.h"
#include "SphereVerticesBinder.h"
#include "MousePositionHistory.h"
#include "ImageRenderer.h"
#include "FullImageRenderer.h"
#include "PlaneVerticesBinder.h"

//*************************************
// global constants
static const char*	window_name = "game";
static const char*	vert_shader_path = "shaders/trackball.vert";
static const char*	frag_shader_path = "shaders/trackball.frag";
static const vec3	initial_pos = vec3(0, 0, 100.0f);
//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size;

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program

//*************************************
// global variables
int		frame = 0;				// index of rendering frames
float prev_time;
struct {
	bool left = false,
		right = false,
		up = false,
		down = false;
	operator bool() const {
		return left || right || up || down;
	}
}mov_key;
float gravity = -98 * 20.0f;
bool onGround = false;
bool portal_switch = true;
bool blue_bullet_switch = true;
bool orange_bullet_switch = true;
bool portal_trans = false;
float jump_power = 350;
float upward_speed = 0;
float upward_backup = 0;
float timer = 0.0f;
bool time_catcher = false;
float b_timer = 0.0f;
bool b_time_catcher = false;
float o_timer = 0.0f;
bool o_time_catcher = false;
int stage = 0;
vec3 prev_loc;
vec4 trans_vec;
bool is_start_page = false;
bool is_start_help_page = false;
bool is_help_page = false;
float moving_time;

//*************************************
// scene objects
Camera*		camera = nullptr;
Light* light = nullptr;
Material* default_material;
ViewProjectionMatrix* view_projection_matrix = nullptr;
MousePositionHistory* mouse_position_history = nullptr;
bool is_left_shift_pressed = false;
bool is_left_ctrl_pressed = false;
bool is_polygon_mode = false;

//*************************************

BindedVertexInfo* sphere_vertex_info;
BindedVertexInfo* block_vertex_info;
BindedVertexInfo* portalx_vertex_info;
BindedVertexInfo* portaly_vertex_info;
BindedVertexInfo* portalz_vertex_info;
BindedVertexInfo* plane_vertex_info;
BindedTextureInfo* box_texture_info;
BindedTextureInfo* black_box_texture_info;
BindedTextureInfo* blue_portal_texture_info;
BindedTextureInfo* orange_portal_texture_info;
BindedTextureInfo* blue_bullet_texture_info;
BindedTextureInfo* orange_bullet_texture_info;
BindedTextureInfo* clear_portal_texture_info;
BindedTextureInfo* enemy_texture_info;
BindedTextureInfo* start_page_texture_info;
BindedTextureInfo* help_page_texture_info;
BindedTextureInfo* end_page_texture_info;
BindedTextureInfo* aim_texture_info;
//*************************************

GameAimingObject* sphere = nullptr;
GameObject* temp_portal_b = nullptr;
GameObject* temp_portal_o = nullptr;
std::vector<GameObject*> blocks;
std::vector<GameMovingObject*> moving_objects;
GameMovingObject* blue_bullet = nullptr;
GameMovingObject* yellow_bullet = nullptr;

//*************************************

std::vector<Renderer*> renderers;
Renderer* blue_bullet_renderer = nullptr;
Renderer* yellow_bullet_renderer = nullptr;
Renderer* blue_portal_renderer = nullptr;
Renderer* yellow_portal_renderer = nullptr;
Renderer* start_page_image_renderer = nullptr;
Renderer* help_page_image_renderer = nullptr;
ImageRenderer* aim_renderer = nullptr;

//*************************************
void change_game_object_direction(GameAimingObject* game_object, vec2 mouse_path);
void initialize_next_stage();
void initialize_start_page();
void initialize_stage1();
void initialize_stage2();
void initialize_stage3();
void initial_end_stage();
void delete_game();
void reset();

void open_start_page();
void close_start_page();
void open_help_page();
void close_help_page();

void change_aim_by_mouse(GameAimingObject* game_object, vec2 mouse_path)
{
	float x_threshold = 0.0001f;
	float y_threhold = 0.0001f;

	if (mouse_path.x >= x_threshold || mouse_path.x <= -x_threshold)
	{
		game_object->set_theta(game_object->get_theta() - mouse_path.x * 0.5f);
	}

	if(mouse_path.y >= y_threhold || mouse_path.y <= -y_threhold)
	{
		game_object->set_alpha(game_object->get_alpha() - mouse_path.y * 0.5f);
	}
}

void dangle_canera_to_game_object(Camera* cam, GameAimingObject* game_object)
{
	vec3 location = game_object->get_location();
	vec3 up = game_object->get_up();
	vec3 forward = game_object->get_forward();
	float alpha = game_object->get_alpha();

	vec3 camera_position_offset = vec3(0.0f, 0.0f, sphere->get_scale().z);
	vec4 new_up = mat4::rotate(cross(up, forward), alpha) * vec4(up, 0.0f);
	vec4 new_at = mat4::rotate(cross(up, forward), alpha) * vec4(forward, 0.0f);

	cam->set_eye(location + camera_position_offset);
	cam->set_up(vec3(new_up.x, new_up.y, new_up.z));
	cam->set_at(location + camera_position_offset + vec3(new_at.x, new_at.y, new_at.z) * 1000.0f);
}

vec2 cursor_to_ndc(dvec2 cursor, ivec2 window_size)
{
	// normalize window pos to [0,1]^2
	vec2 npos = vec2(float(cursor.x) / float(window_size.x - 1),
		float(cursor.y) / float(window_size.y - 1));

	// normalize window pos to [-1,1]^2 with vertical flipping
	// vertical flipping: window coordinate system defines y from
	// top to bottom, while the trackball from bottom to top
	return vec2(npos.x * 2.0f - 1.0f, 1.0f - npos.y * 2.0f);
}

void sphere_movement(float moving_time)
{
	float speed = 450.0f;
	vec3 forward = sphere->get_forward();
	vec3 velocity = vec3(0.0f, 0.0f, 0.0f);
	if (mov_key.up) {
		vec3 loc = sphere->get_location();
		velocity = vec3(forward.x, forward.y, 0.0f).normalize() * (is_left_shift_pressed?1.5f:1.0f);
		sphere->set_location(loc + velocity * moving_time * speed);
	}
	if (mov_key.down) {
		vec3 loc = sphere->get_location();
		velocity = -vec3(forward.x, forward.y, 0.0f).normalize()/2 * (is_left_shift_pressed ? 1.5f : 1.0f);
		sphere->set_location(loc + velocity * moving_time * speed);
	}
	if (mov_key.left) {
		vec3 loc = sphere->get_location();
		velocity = vec3(-forward.y, forward.x, 0.0f).normalize()/2 * (is_left_shift_pressed ? 1.5f : 1.0f);
		sphere->set_location(loc + velocity * moving_time * speed);
	}
	if (mov_key.right) {
		vec3 loc = sphere->get_location();
		velocity = vec3(forward.y, -forward.x, 0.0f).normalize()/2 * (is_left_shift_pressed ? 1.5f : 1.0f);
		sphere->set_location(loc + velocity * moving_time * speed);
	}
}


void gravity_handler(float moving_time) {
	upward_speed += gravity * moving_time;
	if (upward_speed < -30) {
		onGround = false;
	}
	vec3 loc = sphere->get_location();
	sphere->set_location(loc + vec3(0, 0, upward_speed*moving_time));
	if (portal_trans) {
		loc = sphere->get_location();
		trans_vec.a += gravity * moving_time;
		sphere->set_location(loc + vec3(trans_vec.x,trans_vec.y,trans_vec.z)*trans_vec.a/200);
		if (trans_vec.a < 0 || onGround) {
			portal_trans = false;
		}
	}
	
}

void portal_dynamics(GameObject* from, GameObject* to) {
	sphere->set_location(to->get_location() + (sphere->get_scale().z+8.0f)*to->get_up() );
	portal_switch = false;
	if (to->get_up() != vec3(0, 0, 1)) {
		vec3 std_vec = vec3(0, 1, 0);
		float delta_theta = (std_vec.x * to->get_up().y - std_vec.y * to->get_up().x) > 0.0f ?
			acos(std_vec.dot(to->get_up())) :
			-acos(std_vec.dot(to->get_up()));
		sphere->set_theta(delta_theta);
	}
	if (from->get_up() == vec3(0, 0, 1)) {
		if (to->get_up() == vec3(0, 0, 1)) {
			sphere->set_z(sphere->get_location().z + 20.0f);
			upward_speed = std::max(abs(upward_speed), abs(upward_backup));
		}
		else {
			trans_vec = vec4(to->get_up().x, to->get_up().y, to->get_up().z, -upward_speed);
			upward_speed = 0;
			portal_trans = true;
		}
	}
	else {
		if (to->get_up() == vec3(0, 0, 1)) {
			vec3 loc = sphere->get_location() - prev_loc;
			upward_speed = 500 + abs(loc.length())/10;
		}
	}
}

bool find_collistion(GameObject* game_object, GameObject* block)
{
	vec3 loc = game_object->get_location();
	vec3 scale = game_object->get_scale();
	vec3 b_loc = block->get_location();
	vec3 b_scale = block->get_scale();

	if (abs(loc.x - b_loc.x) < b_scale.x / 2 + scale.x /2&& abs(loc.y - b_loc.y) < b_scale.y / 2 + scale.y / 2 && abs(loc.z - b_loc.z) < b_scale.z / 2 + scale.z / 2) return true;
	return false;
}

vec3 find_collision_n_of_block(GameObject* game_object, GameObject* block)
{
	vec3 game_object_location = game_object->get_location();
	vec3 block_location = block->get_location();
	vec3 block_scale = block->get_scale();

	float distance[6];
	distance[0] = abs(game_object_location.x - (block_location.x - block_scale.x / 2));
	distance[1] = abs(game_object_location.x - (block_location.x + block_scale.x / 2));
	distance[2] = abs(game_object_location.y - (block_location.y - block_scale.y / 2));
	distance[3] = abs(game_object_location.y - (block_location.y + block_scale.y / 2));
	distance[4] = abs(game_object_location.z - (block_location.z - block_scale.z / 2));
	distance[5] = abs(game_object_location.z - (block_location.z + block_scale.z / 2));

	float min_distance = distance[0];
	int min_index = 0;
	for (int i = 1; i < 6; i++)
	{
		if (min_distance > distance[i])
		{
			min_distance = distance[i];
			min_index = i;
		}
	}
	if (min_index == 0) return vec3(-1.0f, 0.0f, 0.0f);
	if (min_index == 1) return vec3(1.0f, 0.0f, 0.0f);
	if (min_index == 2) return vec3(0.0f, -1.0f, 0.0f);
	if (min_index == 3) return vec3(0.0f, 1.0f, 0.0f);
	if (min_index == 4) return vec3(0.0f, 0.0f, -1.0f);
	if (min_index == 5) return vec3(0.0f, 0.0f, 1.0f);

	return vec3(0.0f, 1.0f, 0.0f);
}

void collision_handler() {
	vec3 loc = sphere->get_location();
	vec3 moving_vector = loc - prev_loc;
	moving_vector.z = 0;
	for (auto& b : blocks)
	{
		vec3 b_loc = b->get_location();
		vec3 b_scale = b->get_scale();
		if ((b->get_type() == 1 || b->get_type() == 2) && temp_portal_o != nullptr && temp_portal_b != nullptr) {
			if (abs(loc.x - b_loc.x) < b_scale.x/2 + sphere->get_scale().x + abs(b->get_up().x * 5.0f)) {
				if (abs(loc.y - b_loc.y) < b_scale.y/2 + sphere->get_scale().y + abs(b->get_up().y * 5.0f)) {
					if (abs(loc.z - b_loc.z) < b_scale.z/2 + sphere->get_scale().z + abs(b->get_up().z * 10.0f)) {
						if (b->get_type() == 1 && temp_portal_o != nullptr && temp_portal_b != nullptr) {
							if (portal_switch) {
								portal_dynamics(b, temp_portal_o);
								break;
							}
						}
						else if (b->get_type() == 2 && temp_portal_b != nullptr && temp_portal_o != nullptr) {
							if (portal_switch) {
								portal_dynamics(b, temp_portal_b);
								break;
							}
						}
					}
				}
			}
		}
		else {
			if (abs(loc.x - b_loc.x) < b_scale.x / 2 + sphere->get_scale().x) {
				if (abs(loc.y - b_loc.y) < b_scale.y / 2 + sphere->get_scale().y) {
					if (abs(loc.z - b_loc.z) < b_scale.z / 2 + sphere->get_scale().z) {
						if (b->get_type() == 0 || b->get_type() == 3) {
							if (prev_loc.z - b_loc.z > b_scale.z / 2 + sphere->get_scale().z) {
								onGround = true;
								upward_backup = upward_speed;
								upward_speed = 0;
								sphere->set_z(b_loc.z + b_scale.z / 2 + sphere->get_scale().z + 0.1f);
								break;
							}
							else {
								sphere->set_location(loc - moving_vector * 1.001f);
								break;
							}
						}
						else if (b->get_type() == 4) {
							sphere->set_theta(PI * 3 / 2);
							delete_game();
							initialize_next_stage();
							break;
						}
						else if (b->get_type() == 5) {
							reset();
							break;
						}

					}
				}
			}
		}
	}
}


void jump() {
	if (onGround) {
		upward_speed = jump_power;
		onGround = false;
	}
}

void reset()
{
	sphere->set_location(initial_pos);
	sphere->set_theta(PI*3/2);
	temp_portal_b = nullptr;
	temp_portal_o = nullptr;
	delete blue_portal_renderer;
	blue_portal_renderer = nullptr;
	delete yellow_portal_renderer;
	yellow_portal_renderer = nullptr;
	delete blue_bullet;
	blue_bullet = nullptr;
	delete blue_bullet_renderer;
	blue_bullet_renderer = nullptr;
	delete yellow_bullet;
	yellow_bullet = nullptr;
	delete yellow_bullet_renderer;
	yellow_bullet_renderer = nullptr;
}

void update()
{
	float current_time = float(glfwGetTime()) * 0.4f;
	moving_time = current_time - prev_time;
	prev_time = current_time;
	if (!is_start_page && !is_help_page && !is_start_help_page)
	{
		prev_loc = sphere->get_location();
		if(mov_key) sphere_movement(moving_time);
		gravity_handler(moving_time);

		for (auto& b : blocks) {
			collision_handler();
		}

		if (!portal_switch) {
			if (!time_catcher) {
				timer = current_time;
				time_catcher = true;
			}
			else {
				if (current_time - timer > 0.15f) {
					portal_switch = true;
					time_catcher = false;
					timer = 0.0f;
				}
			}
		}

		if (!blue_bullet_switch) {
			if (!b_time_catcher) {
				b_timer = current_time;
				b_time_catcher = true;
			}
			else {
				if (current_time - b_timer > 1.0f) {
					delete blue_bullet;
					blue_bullet = nullptr;
					delete blue_bullet_renderer;
					blue_bullet_renderer = nullptr;
					blue_bullet_switch = true;
					b_time_catcher = false;
					b_timer = 0.0f;
				}
			}
		}

		if (!orange_bullet_switch) {
			if (!o_time_catcher) {
				o_timer = current_time;
				o_time_catcher = true;
			}
			else {
				if (current_time - o_timer > 1.0f) {
					delete yellow_bullet;
					yellow_bullet = nullptr;
					delete yellow_bullet_renderer;
					yellow_bullet_renderer = nullptr;
					orange_bullet_switch = true;
					o_time_catcher = false;
					o_timer = 0.0f;
				}
			}
		}
		vec2 current_position = mouse_position_history->get_current_position();
		vec2 prev_position = mouse_position_history->get_prev_position();
		if (current_position != prev_position) {
			change_aim_by_mouse(sphere, current_position - prev_position);
			mouse_position_history->make_prev_position();
		}
		if (sphere->get_location().z < -500.0f) {
			reset();
		}
		dangle_canera_to_game_object(camera, sphere);
		view_projection_matrix->change_view_matrix(*camera);

		if (blue_bullet)
		{
			for (auto& block : blocks)
			{
				if (block->get_type() == 0  && find_collistion(blue_bullet, block))
				{
					vec3 portal_up = find_collision_n_of_block(blue_bullet, block);
					vec3 portal_location = blue_bullet->get_location() - blue_bullet->get_location() * portal_up * portal_up + block->get_location() * portal_up * portal_up + portal_up * block->get_scale() / 2 + portal_up * vec3(2.5f, 2.5f, 2.5f);
					vec3 portal_scale = vec3(80.0f, 80.0f, 80.0f) - portal_up * portal_up * vec3(75.0f, 75.0f, 75.0f);
				
					temp_portal_b = new GameObject(portal_location, portal_up, 0.0f, portal_scale, 1);
					blocks.push_back(temp_portal_b);
;					if (portal_up == vec3(1, 0, 0) || portal_up == vec3(-1,0,0)) {
						blue_portal_renderer = new BlockRenderer(portalx_vertex_info, blue_portal_texture_info, temp_portal_b, default_material);
					}
					else if (portal_up == vec3(0, 1, 0) || portal_up == vec3(0, -1, 0)) {
						blue_portal_renderer = new BlockRenderer(portaly_vertex_info, blue_portal_texture_info, temp_portal_b, default_material);
					}
					else if (portal_up == vec3(0, 0, 1) || portal_up == vec3(0, 0, -1)) {
						blue_portal_renderer = new BlockRenderer(portalz_vertex_info, blue_portal_texture_info, temp_portal_b, default_material);
					}
				

					delete blue_bullet;
					blue_bullet = nullptr;
					delete blue_bullet_renderer;
					blue_bullet_renderer = nullptr;
					blue_bullet_switch = true;
					b_time_catcher = false;
					b_timer = 0.0f;
					break;
				}
				else if ((block->get_type() == 3 || block->get_type() == 4 || block->get_type() == 5) && find_collistion(blue_bullet, block))
				{
					delete blue_bullet;
					blue_bullet = nullptr;
					delete blue_bullet_renderer;
					blue_bullet_renderer = nullptr; 
					blue_bullet_switch = true;
					b_time_catcher = false;
					b_timer = 0.0f;
					break;
				}
			}
		}
		if (yellow_bullet)
		{
			for (auto& block : blocks)
			{
				if (block->get_type() == 0 && find_collistion(yellow_bullet, block))
				{
					vec3 portal_up = find_collision_n_of_block(yellow_bullet, block);
					vec3 portal_location = yellow_bullet->get_location() - yellow_bullet->get_location() * portal_up * portal_up + block->get_location() * portal_up * portal_up + portal_up * block->get_scale() / 2 + portal_up * vec3(2.5f, 2.5f, 2.5f);
					vec3 portal_scale = vec3(80.0f, 80.0f, 80.0f) - portal_up * portal_up * vec3(75.0f, 75.0f, 75.0f);
					temp_portal_o = new GameObject(portal_location, portal_up, 0.0f, portal_scale, 2);
					blocks.push_back(temp_portal_o);
					if (portal_up == vec3(1, 0, 0) || portal_up == vec3(-1, 0, 0)) {
						yellow_portal_renderer = new BlockRenderer(portalx_vertex_info, orange_portal_texture_info, temp_portal_o, default_material);
					}
					else if (portal_up == vec3(0, 1, 0) || portal_up == vec3(0, -1, 0)) {
						yellow_portal_renderer = new BlockRenderer(portaly_vertex_info, orange_portal_texture_info, temp_portal_o, default_material);
					}
					else if (portal_up == vec3(0, 0, 1) || portal_up == vec3(0, 0, -1)) {
						yellow_portal_renderer = new BlockRenderer(portalz_vertex_info, orange_portal_texture_info, temp_portal_o, default_material);
					}
					delete yellow_bullet;
					yellow_bullet = nullptr;
					delete yellow_bullet_renderer;
					yellow_bullet_renderer = nullptr;
					orange_bullet_switch = true;
					o_time_catcher = false;
					o_timer = 0.0f;
					break;
				}
				else if ((block->get_type() == 3 || block->get_type() == 4 || block->get_type() == 5)&& find_collistion(yellow_bullet, block))
				{
					delete yellow_bullet;
					yellow_bullet = nullptr;
					delete yellow_bullet_renderer;
					yellow_bullet_renderer = nullptr;
					orange_bullet_switch = true;
					o_time_catcher = false;
					o_timer = 0.0f;

					break;
				}
			}
		}

		if (blue_bullet)
		{
			blue_bullet->move(moving_time);
		}
		if (yellow_bullet)
		{
			yellow_bullet->move(moving_time);
		}
	}
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	if (is_start_page)
	{
		start_page_image_renderer->render(program);
	}
	else if (is_help_page)
	{
		help_page_image_renderer->render(program);
	}
	else
	{
		glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_TRUE, view_projection_matrix->get_view_matrix());
		glUniformMatrix4fv(glGetUniformLocation(program, "projection_matrix"), 1, GL_TRUE, view_projection_matrix->get_projection_matrix());

		// setup light properties
		glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light->get_position());
		glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light->get_ambient());
		glUniform4fv(glGetUniformLocation(program, "Id"), 1, light->get_diffuse());
		glUniform4fv(glGetUniformLocation(program, "Is"), 1, light->get_specular());

		for (auto& renderer : renderers)
		{
			renderer->render(program);
		}

		if (blue_bullet_renderer) blue_bullet_renderer->render(program);
		if (yellow_bullet_renderer) yellow_bullet_renderer->render(program);
		if (blue_portal_renderer) blue_portal_renderer->render(program);
		if (yellow_portal_renderer) yellow_portal_renderer->render(program);

		if (aim_renderer)
		{
			aim_renderer->set_window_size(window_size);
			aim_renderer->render(program);
		}
	}

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
	view_projection_matrix->change_projection_matrix(window_size, *camera);
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf("- press F1 to see help\n");
	printf("- press r to reset stage\n");
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{

	if (action == GLFW_PRESS)
	{
		if (is_start_page || is_start_help_page)
		{
			if (key == GLFW_KEY_SPACE)
			{
				if (!is_start_help_page)
				{
					is_start_help_page = true;
					close_start_page();
					open_help_page();
				}
				else
				{
					is_start_help_page = false;
					close_help_page();
					delete_game();
					initialize_next_stage();
				}
			}
			else if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (is_help_page)
		{
			
		}
		else {
			if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
			else if (key == GLFW_KEY_H || key == GLFW_KEY_F1) { open_help_page(); }
			else if (key == GLFW_KEY_LEFT_SHIFT) is_left_shift_pressed = true;
			else if (key == GLFW_KEY_LEFT_CONTROL) is_left_ctrl_pressed = true;
			else if (key == GLFW_KEY_R)	reset();
			else if (key == GLFW_KEY_W)	mov_key.up = true;
			else if (key == GLFW_KEY_A)	mov_key.left = true;
			else if (key == GLFW_KEY_S)	mov_key.down = true;
			else if (key == GLFW_KEY_D)	mov_key.right = true;
			else if (key == GLFW_KEY_SPACE)
			{
				jump();
			}
		}
		
	}
	else if (action == GLFW_RELEASE)
	{
		if (is_start_page || is_start_help_page)
		{

		}
		else if (is_help_page)
		{
			if (key == GLFW_KEY_H || key == GLFW_KEY_F1) close_help_page();
		}
		else
		{
			if (key == GLFW_KEY_LEFT_SHIFT) is_left_shift_pressed = false;
			else if (key == GLFW_KEY_LEFT_CONTROL) is_left_ctrl_pressed = false;
			else if (key == GLFW_KEY_W)	mov_key.up = false;
			else if (key == GLFW_KEY_A)	mov_key.left = false;
			else if (key == GLFW_KEY_S)	mov_key.down = false;
			else if (key == GLFW_KEY_D)	mov_key.right = false;
		}
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if (is_start_page || is_start_help_page) {

	}
	else if (is_help_page) {

	}
	else {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && blue_bullet == nullptr && blue_bullet_renderer == nullptr)
		{
			delete blue_portal_renderer;
			blue_portal_renderer = nullptr;
			temp_portal_b = nullptr;
			blue_bullet_switch = false;
			vec3 location = camera->get_eye();
			vec3 moving_direction = camera->get_at() - location;
			blue_bullet = new GameMovingObject(location, { 0.0f, 0.0f, 1.0f }, 0.0f, { 3.0f, 3.0f, 3.0f }, 0, moving_direction * 0.02f);
			blue_bullet_renderer = new SphereRenderer(sphere_vertex_info, blue_bullet_texture_info, blue_bullet, default_material);
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && yellow_bullet == nullptr && yellow_bullet_renderer == nullptr)
		{
			delete yellow_portal_renderer;
			yellow_portal_renderer = nullptr;
			temp_portal_o = nullptr;
			orange_bullet_switch = false;
			vec3 location = camera->get_eye();
			vec3 moving_direction = camera->get_at() - location;
			yellow_bullet = new GameMovingObject(location, { 0.0f, 0.0f, 1.0f }, 0.0f, { 3.0f, 3.0f, 3.0f }, 0, moving_direction * 0.02f);
			yellow_bullet_renderer = new SphereRenderer(sphere_vertex_info, orange_bullet_texture_info, yellow_bullet, default_material);
		}
	} 
}

void motion( GLFWwindow* window, double x, double y )
{
	if (is_start_page || is_start_help_page) {

	}
	else if (is_help_page) {

	}
	else {
		vec2 current_position = cursor_to_ndc(dvec2(x, y), window_size);
		mouse_position_history->change_position(current_position);
	}
}

void user_init()
{
	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable(GL_BLEND);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void user_finalize()
{

}

void create_graphic_object()
{
	sphere_vertex_info = SphereVerticesBinder::bind();
	block_vertex_info = BlockVerticesBinder::bind();
	portalx_vertex_info = PortalXVerticesBinder::bind();
	portaly_vertex_info = PortalYVerticesBinder::bind();
	portalz_vertex_info = PortalZVerticesBinder::bind();
	plane_vertex_info = PlaneVerticesBinder::bind();

	box_texture_info = TextureBinder::bind("textures/box.png");
	black_box_texture_info = TextureBinder::bind("textures/black_box.png");
	blue_portal_texture_info = TextureBinder::bind("textures/blue_portal.png");
	orange_portal_texture_info = TextureBinder::bind("textures/orange_portal.png");
	blue_bullet_texture_info = TextureBinder::bind("textures/blue_bullet.png");
	orange_bullet_texture_info = TextureBinder::bind("textures/orange_bullet.png");
	clear_portal_texture_info = TextureBinder::bind("textures/clear.png");
	enemy_texture_info = TextureBinder::bind("textures/enemy.png");
	start_page_texture_info = TextureBinder::bind("textures/start_page.png");
	help_page_texture_info = TextureBinder::bind("textures/help_page.png");
	end_page_texture_info = TextureBinder::bind("textures/end_page.png");
	aim_texture_info = TextureBinder::bind("textures/aim.png");


	default_material = new Material(
		vec4(0.5f, 0.5f, 0.5f, 1.0f),
		vec4(0.8f, 0.8f, 0.8f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		1000.0f
	);
}

void initialize_next_stage()
{
	if (stage == 0)
	{
		initialize_stage1();
	}
	else if (stage == 1)
	{
		initialize_stage2();
	}
	else if (stage == 2)
	{
		initialize_stage3();
	}
	else {
		initial_end_stage();
	}
}

void initialize_start_page()
{
	start_page_image_renderer = new FullImageRenderer(plane_vertex_info, start_page_texture_info);
}

void initialize_help_page()
{
	help_page_image_renderer = new FullImageRenderer(plane_vertex_info, help_page_texture_info);
}

void initialize_stage1()
{
	stage = 1;
	camera = new Camera();
	view_projection_matrix = new ViewProjectionMatrix(window_size, *camera);
	mouse_position_history = new MousePositionHistory();
	light = new Light(
		vec4(0.0f, 0.0f, 100.0f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		vec4(0.8f, 0.8f, 0.8f, 1.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f)
	);

	GameObject* plane;
	plane = new GameObject({ 0.0f, 0.0f, -40.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 1000.0f, 1000.0f, 50.0f }, 0);
	blocks.push_back(plane);
	plane = new GameObject({ -900.0f, 0.0f, -40.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 200.0f, 1000.0f, 50.0f }, 0);
	blocks.push_back(plane);


	GameObject* box;
	box = new GameObject({ 200.0f, 100.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 100.0f, 150.0f, 20.0f }, 0);
	blocks.push_back(box);
	box = new GameObject({ -200.0f, -100.0f, 20.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 100.0f, 150.0f, 80.0f }, 0);
	blocks.push_back(box);
	box = new GameObject({ 200.0f, -100.0f, 20.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 100.0f, 150.0f, 80.0f }, 0);
	blocks.push_back(box);

	GameObject* wall;
	wall = new GameObject({ -1000.0f, 0.0f, 220.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 30.0f, 1000.0f, 530.0f }, 0);
	blocks.push_back(wall);
	wall = new GameObject({ 500.0f, 0.0f, 220.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 30.0f, 1000.0f, 530.0f }, 0);
	blocks.push_back(wall);

	wall = new GameObject({ -500.0f, 500.0f, 70.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 2000.0f, 30.0f, 830.0f }, 0);
	blocks.push_back(wall);
	wall = new GameObject({ -500.0f, -500.0f, 70.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 2000.0f, 30.0f, 830.0f }, 0);
	blocks.push_back(wall);

	GameObject* black_wall;
	black_wall = new GameObject({ -795.0f, 0.0f, -165.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 10.0f, 1000.0f, 300.0f }, 3);
	blocks.push_back(black_wall);
	black_wall = new GameObject({ -505.0f, 0.0f, -165.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 10.0f, 1000.0f, 300.0f }, 3);
	blocks.push_back(black_wall);

	GameObject* goal;
	goal = new GameObject({ -970.0f, 0.0f, 30.0f }, { 1.0f, 0.0f, 0.0f }, 0.0f, { 10.0f, 80.0f, 120.0f }, 4);
	blocks.push_back(goal);

	sphere = new GameAimingObject(initial_pos, { 0.0f, 0.0f, 1.0f }, PI*3/2, { 10.0f, 10.0f, 10.0f }, 0, vec3(0.0f, 0.0f, 0.0f), 0.0f);

	SphereRenderer* sphereRenderer = new SphereRenderer(sphere_vertex_info, box_texture_info, sphere, default_material);
	renderers.push_back(sphereRenderer);

	for (auto& b : blocks) {
		int texture_type = b->get_type();
		if (texture_type == 0) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, box_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
		else if (texture_type == 3) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, black_box_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
		else if (texture_type == 4) {
			if (b->get_up() == vec3(1, 0, 0) || b->get_up() == vec3(-1, 0, 0)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portalx_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
			else if (b->get_up() == vec3(0, 1, 0) || b->get_up() == vec3(0, -1, 0)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portaly_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
			else if (b->get_up() == vec3(0, 0, 1) || b->get_up() == vec3(0, 0, -1)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portalz_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
		}
		else if (texture_type == 5) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, enemy_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
	}
}

void initialize_stage2()
{
	stage = 2;
	camera = new Camera();
	view_projection_matrix = new ViewProjectionMatrix(window_size, *camera);
	mouse_position_history = new MousePositionHistory();
	light = new Light(
		vec4(0.0f, 0.0f, 100.0f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		vec4(0.8f, 0.8f, 0.8f, 1.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f)
	);

	sphere = new GameAimingObject(initial_pos + vec3(0, 0, 0), { 0.0f, 0.0f, 1.0f }, PI*3/2, { 10.0f, 10.0f, 10.0f }, 0, vec3(0.0f, 0.0f, 0.0f), 0.0f);
	SphereRenderer* sphereRenderer = new SphereRenderer(sphere_vertex_info, box_texture_info, sphere, default_material);
	renderers.push_back(sphereRenderer);

	GameObject* plane;
	plane = new GameObject({ 200.0f, 0.0f, -30.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 1000.0f, 500.0f, 30.0f }, 0);
	blocks.push_back(plane);
	plane = new GameObject({ -110.0f, 0.0f, 500.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 180.0f, 500.0f, 30.0f }, 0);
	blocks.push_back(plane);


	GameObject* black_plane;
	black_plane = new GameObject({ -17.5f, 0.0f, 500.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 10.0f, 500.0f, 32.0f }, 3);
	blocks.push_back(black_plane);
	black_plane = new GameObject({ -100.0f, 0.0f, 270.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 200.0f, 500.0f, 30.0f }, 3);
	blocks.push_back(black_plane);
	black_plane = new GameObject({ 520.0f, 0.0f, 280.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 400.0f, 500.0f, 30.0f }, 3);
	blocks.push_back(black_plane);
	black_plane = new GameObject({ 200.0f, 0.0f, 670.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 1000.0f, 500.0f, 30.0f }, 3);
	blocks.push_back(black_plane);



	GameObject* black_wall;
	black_wall = new GameObject({ 225.0f, 200.0f, 320.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 800.0f, 10.0f, 700.0f }, 3);
	blocks.push_back(black_wall);
	black_wall = new GameObject({ 225.0f, -200.0f, 320.0f }, { 0.0f, 0.0f, 1.0f }, PI, { 800.0f, 10.0f, 700.0f }, 3);
	blocks.push_back(black_wall);
	black_wall = new GameObject({ -150.0f, 0.0f, 320.0f }, { 0.0f, 0.0f, 1.0f }, PI, { 10.0f, 500.0f, 700.0f }, 3);
	blocks.push_back(black_wall);
	black_wall = new GameObject({ 600.0f, 0.0f, 320.0f }, { 0.0f, 0.0f, 1.0f }, PI, { 10.0f, 500.0f, 700.0f }, 3);
	blocks.push_back(black_wall);

	GameObject* goal;
	goal = new GameObject({ 520.0f, 0.0f, 360.0f }, { 1.0f, 0.0f, 0.0f }, 0.0f, { 10.0f, 80.0f, 120.0f }, 4);
	blocks.push_back(goal);


	for (auto& b : blocks) {
		int texture_type = b->get_type();
		if (texture_type == 0) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, box_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
		else if (texture_type == 3) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, black_box_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
		else if (texture_type == 4) {
			if (b->get_up() == vec3(1, 0, 0) || b->get_up() == vec3(-1, 0, 0)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portalx_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
			else if (b->get_up() == vec3(0, 1, 0) || b->get_up() == vec3(0, -1, 0)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portaly_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
			else if (b->get_up() == vec3(0, 0, 1) || b->get_up() == vec3(0, 0, -1)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portalz_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
		}
		else if (texture_type == 5) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, enemy_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
	}
}

void initialize_stage3()
{
	stage = 3;
	camera = new Camera();
	view_projection_matrix = new ViewProjectionMatrix(window_size, *camera);
	mouse_position_history = new MousePositionHistory();
	light = new Light(
		vec4(400.0f, 0.0f, 200.0f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		vec4(0.8f, 0.8f, 0.8f, 1.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f)
	);

	sphere = new GameAimingObject(initial_pos + vec3(0, 0, 0), { 0.0f, 0.0f, 1.0f }, PI * 3 / 2, { 10.0f, 10.0f, 10.0f }, 0, vec3(0.0f, 0.0f, 0.0f), 0.0f);
	SphereRenderer* sphereRenderer = new SphereRenderer(sphere_vertex_info, box_texture_info, sphere, default_material);
	renderers.push_back(sphereRenderer);

	GameObject* plane;
	GameObject* black_plane;

	plane = new GameObject({ 0.0f, 0.0f, -30.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 1000.0f, 600.0f, 30.0f }, 0);
	blocks.push_back(plane);
	plane = new GameObject({ 750.0f, 0.0f, 170.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 500.0f, 600.0f, 30.0f }, 0);
	blocks.push_back(plane);
	black_plane = new GameObject({ 2100.0f, 0.0f, -30.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 500.0f, 600.0f, 30.0f }, 3);
	blocks.push_back(black_plane);

	GameObject* enemy;
	enemy = new GameObject({ 300.0f, 200.0f, 30.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 100.0f, 100.0f, 100.0f }, 5);
	blocks.push_back(enemy);



	GameObject* wall;
	wall = new GameObject({ -50.0f, 0.0f, 220.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 30.0f, 600.0f, 530.0f }, 0);
	blocks.push_back(wall);
	wall = new GameObject({ 500.0f, 0.0f, 70.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 30.0f, 600.0f, 200.0f }, 0);
	blocks.push_back(wall);
	wall = new GameObject({ 500.0f, 0.0f, 270.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 30.0f, 200.0f, 200.0f }, 0);
	blocks.push_back(wall);

	wall = new GameObject({ 500.0f, 300.0f, 70.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 1500.0f, 30.0f, 830.0f }, 0);
	blocks.push_back(wall);
	wall = new GameObject({ 500.0f, -300.0f, 70.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 1500.0f, 30.0f, 830.0f }, 0);
	blocks.push_back(wall);

	wall = new GameObject({ 1000.0f, 0.0f, 300.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 30.0f, 300.0f, 800.0f }, 0);
	blocks.push_back(wall);
	GameObject* black_wall;
	black_wall = new GameObject({ 2350.0f, 0.0f, 270.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 30.0f, 600.0f, 600.0f }, 3);
	blocks.push_back(black_wall);
	black_wall = new GameObject({ 2150.0f, -300.0f, 270.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 600.0f, 30.0f, 600.0f }, 3);
	blocks.push_back(black_wall);
	black_wall = new GameObject({ 2150.0f, 300.0f, 270.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 600.0f, 30.0f, 600.0f }, 3);
	blocks.push_back(black_wall);

	GameObject* black_box;
	GameObject* box;

	//
	black_box = new GameObject({ 1200.0f, 0.0f, 50.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 100.0f, 100.0f, 100.0f }, 3);
	blocks.push_back(black_box);
	enemy = new GameObject({ 1300.0f, 0.0f, 50.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 100.0f, 100.0f, 100.0f }, 5);
	blocks.push_back(enemy);
	box = new GameObject({ 1400.0f, 0.0f, 50.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 100.0f, 100.0f, 100.0f }, 0);
	blocks.push_back(box);
	enemy = new GameObject({ 1500.0f, 0.0f, 50.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 100.0f, 100.0f, 100.0f }, 5);
	blocks.push_back(enemy);
	enemy = new GameObject({ 1600.0f, 0.0f, 50.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 100.0f, 100.0f, 100.0f }, 5);
	blocks.push_back(enemy);

	GameObject* goal;
	goal = new GameObject({ 2200.0f, 0.0f, 40.0f }, { 1.0f, 0.0f, 0.0f }, 0.0f, { 10.0f, 80.0f, 120.0f }, 4);
	blocks.push_back(goal);

	

	for (auto& b : blocks) {
		int texture_type = b->get_type();
		if (texture_type == 0) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, box_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
		else if (texture_type == 3) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, black_box_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
		else if (texture_type == 4) {
			if (b->get_up() == vec3(1, 0, 0) || b->get_up() == vec3(-1, 0, 0)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portalx_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
			else if (b->get_up() == vec3(0, 1, 0) || b->get_up() == vec3(0, -1, 0)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portaly_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
			else if (b->get_up() == vec3(0, 0, 1) || b->get_up() == vec3(0, 0, -1)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portalz_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
		}
		else if (texture_type == 5) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, enemy_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
	}
}

void initial_end_stage() {
	stage = 4;
	camera = new Camera();
	view_projection_matrix = new ViewProjectionMatrix(window_size, *camera);
	mouse_position_history = new MousePositionHistory();
	light = new Light(
		vec4(0.0f, 0.0f, 270.0f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		vec4(0.8f, 0.8f, 0.8f, 1.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f)
	);
	sphere = new GameAimingObject(initial_pos + vec3(0, 0, 0), { 0.0f, 0.0f, 1.0f }, PI*3/2, { 10.0f, 10.0f, 10.0f }, 0, vec3(0.0f, 0.0f, 0.0f), 0.0f);
	SphereRenderer* sphereRenderer = new SphereRenderer(sphere_vertex_info, box_texture_info, sphere, default_material);
	renderers.push_back(sphereRenderer);
	GameObject* plane;
	plane = new GameObject({ 0.0f, 0.0f, -30.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 600.0f, 600.0f, 30.0f }, 0);
	blocks.push_back(plane);
	GameObject* wall;
	//wall = new GameObject({ 300.0f, 0.0f, 270.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 30.0f, 600.0f, 600.0f }, 0);
	//blocks.push_back(wall);
	wall = new GameObject({ -300.0f, 0.0f, 270.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 30.0f, 600.0f, 600.0f }, 0);
	blocks.push_back(wall);
	wall = new GameObject({ 0.0f, -300.0f, 270.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 600.0f, 30.0f, 600.0f }, 0);
	blocks.push_back(wall);
	wall = new GameObject({ 0.0f, 300.0f, 270.0f }, { -1.0f, 0.0f, 0.0f }, 0.0f, { 600.0f, 30.0f, 600.0f }, 0);
	blocks.push_back(wall);

	wall = new GameObject({ 500.0f, 0.0f, 300.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 30.0f, 500.0f, 500.0f }, 8);
	blocks.push_back(wall);
	for (auto& b : blocks) {
		int texture_type = b->get_type();
		if (texture_type == 0) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, box_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
		else if (texture_type == 3) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, black_box_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
		else if (texture_type == 4) {
			if (b->get_up() == vec3(1, 0, 0) || b->get_up() == vec3(-1, 0, 0)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portalx_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
			else if (b->get_up() == vec3(0, 1, 0) || b->get_up() == vec3(0, -1, 0)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portaly_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
			else if (b->get_up() == vec3(0, 0, 1) || b->get_up() == vec3(0, 0, -1)) {
				BlockRenderer* blockRenderer = new BlockRenderer(portalz_vertex_info, clear_portal_texture_info, b, default_material);
				renderers.push_back(blockRenderer);
			}
		}
		else if (texture_type == 5) {
			BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, enemy_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
		else if (texture_type == 8) {
			BlockRenderer* blockRenderer = new BlockRenderer(portalx_vertex_info, end_page_texture_info, b, default_material);
			renderers.push_back(blockRenderer);
		}
	}
}

void delete_game()
{
	delete camera;
	camera = nullptr;
	delete view_projection_matrix;
	view_projection_matrix = nullptr;
	delete mouse_position_history;
	mouse_position_history = nullptr;
	delete light;
	light = nullptr;

	for (auto& block : blocks)
	{
		delete block;
		block = nullptr;
	}

	blocks.clear();
	
	delete sphere;
	sphere = nullptr;

	for (auto& renderer : renderers)
	{
		delete renderer;
		renderer = nullptr;
	}

	renderers.clear();


	temp_portal_b = nullptr;
	delete blue_portal_renderer;
	blue_portal_renderer = nullptr;
	temp_portal_o = nullptr;
	delete yellow_portal_renderer;
	yellow_portal_renderer = nullptr;
	delete blue_bullet;
	blue_bullet = nullptr;
	delete blue_bullet_renderer;
	blue_bullet_renderer = nullptr;
	delete yellow_bullet;
	yellow_bullet = nullptr;
	delete yellow_bullet_renderer;
	yellow_bullet_renderer = nullptr;
	is_start_page = false;
}

void open_start_page()
{
	is_start_page = true;
}

void close_start_page()
{
	is_start_page = false;
}

void open_help_page()
{
	is_help_page = true;
}

void close_help_page()
{
	is_help_page = false;
}

int main( int argc, char* argv[] )
{
	window_size = cg_default_window_size(); // initial window size
	window_size = ivec2(960, 720);
	// create window and initialize OpenGL extensions
	//if(!(window = cg_create_window( window_name, window_size.x, window_size.y))){ glfwTerminate(); return 1; }
	if (!(window = glfwCreateWindow(1280, 960, window_name, glfwGetPrimaryMonitor(), NULL))) { glfwTerminate(); return 1; }
	if (!cg_init_extensions(window)) { glfwTerminate(); return 1; }	// version and extensions
	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	user_init();					// user initialization

	create_graphic_object();

	initialize_start_page();
	initialize_help_page();

	aim_renderer = new ImageRenderer(plane_vertex_info,aim_texture_info, window_size, vec2(0.0f, 0.0f), vec2(0.1f, 0.1f), 0.0f);

	open_start_page();

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	print_help();

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
