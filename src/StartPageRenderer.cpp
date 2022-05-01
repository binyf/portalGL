#include "StartPageRenderer.h"

StartPageRenderer::StartPageRenderer(BindedVertexInfo* vertex_info, BindedTextureInfo* texture_info)
	: vertex_info(vertex_info), texture_info(texture_info)
{
}

void StartPageRenderer::render(GLuint program)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture_info->get_texture());
	glUniform1i(glGetUniformLocation(program, "TEX"), 0);
	glUniform1i(glGetUniformLocation(program, "mode"), 6);

	if (this->vertex_info->get_vertex_array()) glBindVertexArray(this->vertex_info->get_vertex_array());

	glDrawElements(GL_TRIANGLES, this->vertex_info->get_index_buffer_size(), GL_UNSIGNED_INT, nullptr);
}
