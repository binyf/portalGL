#include "ImageRenderer.h"

ImageRenderer::ImageRenderer(BindedVertexInfo* vertex_info, BindedTextureInfo* texture_info, ivec2 window_size, vec2 location, vec2 scale, float theta)
	: vertex_info(vertex_info), texture_info(texture_info), window_size(window_size), location(location), scale(scale), theta(theta)
{
}

mat4 ImageRenderer::get_aspect_matrix()
{
	float aspect = this->window_size.x / float(this->window_size.y);
	mat4 aspect_matrix =
	{
		std::min(1 / aspect,1.0f), 0, 0, 0,
		0, std::min(aspect,1.0f), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	return aspect_matrix;
}

mat4 ImageRenderer::get_model_matrix()
{
	return mat4::translate({ this->location.x, this->location.y, 0 })
		* mat4::rotate(vec3(0, 0, 1), this->theta)
		* mat4::scale(vec3(this->scale.x, this->scale.y, 1.0f));
}

void ImageRenderer::set_window_size(ivec2 window_size)
{
	this->window_size = window_size;
}

void ImageRenderer::render(GLuint program)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture_info->get_texture());
	glUniform1i(glGetUniformLocation(program, "TEX"), 0);
	glUniform1i(glGetUniformLocation(program, "mode"), 7);

	glUniformMatrix4fv(glGetUniformLocation(program, "aspect_matrix"), 1, GL_TRUE, this->get_aspect_matrix());
	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, this->get_model_matrix());

	if (this->vertex_info->get_vertex_array()) glBindVertexArray(this->vertex_info->get_vertex_array());

	glDrawElements(GL_TRIANGLES, this->vertex_info->get_index_buffer_size(), GL_UNSIGNED_INT, nullptr);
}
