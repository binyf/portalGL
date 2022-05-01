#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "cgmath.h"

class Material
{
private:
	vec4	ambient;
	vec4	diffuse;
	vec4	specular;
	float	shininess;

public:
	Material(vec4 ambient, vec4 diffuse, vec4 specular, float shininess);
	vec4 get_ambient();
	vec4 get_diffuse();
	vec4 get_specular();
	float get_shininess();

};

#endif
