#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "cgmath.h"
#include "cgut.h"

class Renderer
{
public:
	virtual void render(GLuint program) = 0;
};

#endif
