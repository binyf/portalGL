#ifndef __BLOCK_RENDERER_H__
#define __BLOCK_RENDERER_H__

#include "Renderer.h"
#include "BindedVertexInfo.h"
#include "BindedTextureInfo.h"
#include "GameObject.h"
#include "cgmath.h"
#include "cgut.h"
#include "Material.h"

class BlockRenderer : public Renderer
{
private:
	BindedVertexInfo* vertex_info;
	BindedTextureInfo* texture_info;
	GameObject* object;
	Material* material;

	mat4 get_model_matrix();

public:
	BlockRenderer(
		BindedVertexInfo* vertex_info,
		BindedTextureInfo* texture_info,
		GameObject* object,
		Material* material
	);
	void render(GLuint program) override;
};

#endif
