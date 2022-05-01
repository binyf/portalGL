#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "cgmath.h"

class Camera
{
private:
	vec3	eye;
	vec3	at;
	vec3	up;
	float	fovy;
	float	dnear;
	float	dfar;

	vec3 get_n();
	vec3 get_u();
	vec3 get_v();

public:
	Camera();
	~Camera();
	void initialize();
	vec3 get_eye();
	void set_eye(vec3 eye);
	vec3 get_at();
	void set_at(vec3 at);
	vec3 get_up();
	void set_up(vec3 up);
	float get_fovy();
	float get_dnear();
	float get_dfar();

	void zoom(float size);
	void pan(vec2 position);
	void track_ball(vec2 initial_position, vec2 current_position);
};

#endif
