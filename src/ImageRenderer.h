#ifndef __IMAGE_RENDERER_H__
#define __IMAGE_RENDERER_H__

#include "Renderer.h"
#include "BindedVertexInfo.h"
#include "BindedTextureInfo.h"
#include "GameObject.h"
#include "cgmath.h"
#include "cgut.h"
#include "Material.h"

class ImageRenderer : public Renderer
{
private:
	BindedVertexInfo* vertex_info;
	BindedTextureInfo* texture_info;
	ivec2 window_size;
	vec2 location;
	vec2 scale;
	float theta;

	mat4 get_aspect_matrix();
	mat4 get_model_matrix();

public:
	ImageRenderer(
		BindedVertexInfo* vertex_info,
		BindedTextureInfo* texture_info,
		ivec2 window_size,
		vec2 location,
		vec2 scale,
		float theta
	);

	void set_window_size(ivec2 window_size);
	void render(GLuint program) override;
};

#endif
