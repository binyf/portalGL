#include "GameObject.h"

GameObject::GameObject(vec3 location, vec3 up, float theta, vec3 scale, int type): location(location), up(up.normalize()), theta(theta), scale(scale), type(type)
{

}

vec3 GameObject::get_location()
{
	return this->location;
}

void GameObject::set_location(vec3 location)
{
	this->location = location;
}

vec3 GameObject::get_up()
{
	return this->up;
}

void GameObject::set_up(vec3 up)
{
	this->up = up.normalize();
}

float GameObject::get_theta()
{
	return this->theta;
}

vec3 GameObject::get_forward()
{
	vec3 origin_up = vec3(0.0f, 0.0f, 1.0f);
	vec4 origin_forward = vec4(0.0f, 1.0f, 0.0f, 0.0f);

	float cos = origin_up.dot(this->up);
	vec3 axis = cross(origin_up, up);
	float tan = sqrt(1 - cos * cos) / cos;

	vec4 forward = mat4::rotate(this->up, this->theta) * mat4::rotate(axis, atan(tan)) * origin_forward;
	
	return vec3(forward.x, forward.y, forward.z).normalize();
}

void GameObject::set_theta(float theta)
{
	this->theta = theta;
}

vec3 GameObject::get_scale()
{
	return this->scale;
}

void GameObject::set_scale(vec3 scale)
{
	this->scale = scale;
}

void GameObject::set_z(float z)
{
	this->location.z = z;
}

int GameObject::get_type()
{
	return this->type;
}