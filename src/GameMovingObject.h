#ifndef __GAME_MOVING_OBJECT_H__
#define __GAME_MOVING_OBJECT_H__

#include "GameObject.h"

class GameMovingObject : public GameObject
{
private:
	vec3 velocity;

public:
	GameMovingObject(vec3 location, vec3 up, float theta, vec3 scale, int type, vec3 velocity);
	vec3 get_velocity();
	void set_velocity(vec3 velocity);
	void move(float moving_time);
};

#endif
