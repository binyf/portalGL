#include "GameMovingObject.h"

GameMovingObject::GameMovingObject(vec3 location, vec3 up, float theta, vec3 scale, int type, vec3 velocity)
    : GameObject(location, up, theta, scale, type), velocity(velocity)
{

}

vec3 GameMovingObject::get_velocity()
{
    return this->velocity;
}

void GameMovingObject::set_velocity(vec3 velocity)
{
    this->velocity = velocity;
}

void GameMovingObject::move(float moving_time)
{
    this->location = this->location + this->velocity;
}
