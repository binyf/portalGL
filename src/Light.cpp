#include "Light.h"

Light::Light(vec4 position, vec4 ambient, vec4 diffuse, vec4 spe): position(position), ambient(ambient), diffuse(diffuse), specular(specular)
{
}

vec4 Light::get_position()
{
	return this->position;
}

vec4 Light::get_ambient()
{
	return this->ambient;
}

vec4 Light::get_diffuse()
{
	return this->diffuse;
}

vec4 Light::get_specular()
{
	return this->specular;
}
