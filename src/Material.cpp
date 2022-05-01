#include "Material.h"

Material::Material(vec4 ambient, vec4 diffuse, vec4 specular, float shininess): ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
{
}

vec4 Material::get_ambient()
{
	return this->ambient;
}

vec4 Material::get_diffuse()
{
	return this->diffuse;
}

vec4 Material::get_specular()
{
	return this->specular;
}

float Material::get_shininess()
{
	return this->shininess;
}
