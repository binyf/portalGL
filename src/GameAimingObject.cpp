#include "GameAimingObject.h"

GameAimingObject::GameAimingObject(vec3 location, vec3 up, float theta, vec3 scale, int type, vec3 velocity, float alpha)
	: GameMovingObject(location, up, theta, scale, type, velocity), alpha(alpha)
{
}

float GameAimingObject::get_alpha()
{
	return this->alpha;
}

void GameAimingObject::set_alpha(float alpha)
{
	float large_limit = PI / 2;
	float small_limit = -PI * 40 / 90;
	if (alpha > large_limit) this->alpha = large_limit;
	else if (alpha < small_limit) this->alpha = small_limit;
	else this->alpha = alpha;
}
