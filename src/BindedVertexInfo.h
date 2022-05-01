#ifndef __BINDED_VERTEX_INFO_H__
#define __BINDED_VERTEX_INFO_H__

#include "cgmath.h"
#include "cgut.h"

class BindedVertexInfo
{
private:
	GLuint vertex_buffer;
	GLuint index_buffer;
	GLuint vertex_array;
	uint index_buffer_size;

public:
	BindedVertexInfo(
		GLuint vertex_buffer,
		GLuint index_buffer,
		GLuint vertex_array,
		uint index_buffer_size
	);
	uint get_index_buffer_size();
	GLuint get_vertex_array();
};

#endif
