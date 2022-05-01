#ifndef __BINDED_TEXTURE_INFO_H__
#define __BINDED_TEXTURE_INFO_H__

#include "cgmath.h"
#include "cgut.h"

class BindedTextureInfo
{
private:
	GLuint texture;

public:
	BindedTextureInfo(GLuint texture);
	GLuint get_texture();
};

#endif
