#ifndef __TEXTURE_BINDER_H__
#define __TEXTURE_BINDER_H__

#include "BindedTextureInfo.h"
#include "cgmath.h"
#include "cgut.h"

class TextureBinder
{
public:
	static BindedTextureInfo* bind(const char* image_path);
};

#endif
