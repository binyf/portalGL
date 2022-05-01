#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "cgmath.h"

class Light
{
private:
    vec4	position;
    vec4	ambient;
    vec4	diffuse;
    vec4	specular;

public:
    Light(vec4 position, vec4 ambient, vec4 diffuse, vec4 specular);
    vec4 get_position();
    vec4 get_ambient();
    vec4 get_diffuse();
    vec4 get_specular();
};

#endif
