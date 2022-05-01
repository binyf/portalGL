#include "Camera.h"

vec3 Camera::get_n()
{
	return (this->eye - this->at).normalize();
}

vec3 Camera::get_u()
{
	return cross(this->up, this->get_n()).normalize();
}

vec3 Camera::get_v()
{
	return cross(this->get_n(), this->get_u()).normalize();
}

Camera::Camera()
{
	this->initialize();
}

Camera::~Camera()
{
}

void Camera::initialize()
{
	this->eye = vec3(0, -300, 0);
	this->at = vec3(0, 0, 0);
	this->up = vec3(0, 0, 1);

	this->fovy = PI / 2.5f;
	this->dnear = 1.0f;
	this->dfar = 100000.0f;
}

vec3 Camera::get_eye()
{
	return this->eye;
}

void Camera::set_eye(vec3 eye)
{
	this->eye = eye;
}

vec3 Camera::get_at()
{
	return this->at;
}

void Camera::set_at(vec3 at)
{
	this->at = at;
}

vec3 Camera::get_up()
{
	return this->up;
}

void Camera::set_up(vec3 up)
{
	this->up = up;
}

float Camera::get_fovy()
{
	return this->fovy;
}

float Camera::get_dnear()
{
	return this->dnear;
}

float Camera::get_dfar()
{
	return this->dfar;
}

void Camera::zoom(float size)
{
	vec3 n = this->get_n();
	this->eye -= size * 100 * n;
}

void Camera::pan(vec2 position)
{
	vec3 u = this->get_u();
	vec3 v = this->get_v();
	vec3 n = this->get_n();

	vec3 a = 100 * (u * position.x + v * position.y);

	this->eye -= a;
	this->at -= a;
	
}

void Camera::track_ball(vec2 prev_position, vec2 current_position)
{
	vec2 moving_vector = current_position - prev_position;
	vec3 rotated_moving_vector = vec3(moving_vector.y, -moving_vector.x, 0);
	
	vec3 u = this->get_u();
	vec3 v = this->get_v();
	
	vec3 vv = u * rotated_moving_vector.x + v * rotated_moving_vector.y;

	float theta = sqrtf((current_position.x - prev_position.x)* (current_position.x - prev_position.x) + (current_position.y - prev_position.y) * (current_position.y - prev_position.y));
	vec4 old_eye = vec4(this->eye, 0.0f);
	vec4 old_up = vec4(this->up, 0.0f);
	vec4 new_eye = mat4::rotate(vv.normalize(), theta) * old_eye;
	vec4 new_up = mat4::rotate(vv.normalize(), theta) * old_up;
	this->eye = vec3(new_eye.x, new_eye.y, new_eye.z);
	this->up = vec3(new_up.x, new_up.y, new_up.z);
}
