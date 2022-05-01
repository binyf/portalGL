#ifndef __START_PAGE_RENDERER_H__
#define __START_PAGE_RENDERER_H__

#include "Renderer.h"
#include "BindedVertexInfo.h"
#include "BindedTextureInfo.h"
#include "GameObject.h"
#include "cgmath.h"
#include "cgut.h"
#include "Material.h"

class StartPageRenderer : public Renderer
{
private:
	BindedVertexInfo* vertex_info;
	BindedTextureInfo* texture_info;

public:
	StartPageRenderer(
		BindedVertexInfo* vertex_info,
		BindedTextureInfo* texture_info
	);
	void render(GLuint program) override;
};

#endif
