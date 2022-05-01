#ifndef __SPHERE_VERTICES_BINDER_H__
#define __SPHERE_VERTICES_BINDER_H__

#include "BindedVertexInfo.h"
#include "cgmath.h"
#include "cgut.h"

class SphereVerticesBinder
{
private:
	static vec3 get_vertex_position(float r, float theta, float pi);
	static vec3 get_vertex_normal_vector(float theta, float pi);
	static vec2 get_texture_coordinate(float theta, float pi);

public:
	static BindedVertexInfo* bind();
};

#endif
