#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "cgmath.h"
#include "cgut.h"

class GameObject
{
protected:
	vec3 location;
	vec3 up;
	float theta;
	vec3 scale;
	int type;

public:
	GameObject(vec3 location, vec3 up, float theta, vec3 scale, int type);
	vec3 get_location();
	void set_location(vec3 location);
	vec3 get_up();
	void set_up(vec3 up);
	float get_theta();
	vec3 get_forward();
	void set_theta(float theta);
	vec3 get_scale();
	void set_scale(vec3 scale);
	void set_z(float z);
	int get_type();
};

#endif
