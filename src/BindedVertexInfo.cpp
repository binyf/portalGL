#include "BindedVertexInfo.h"

BindedVertexInfo::BindedVertexInfo(GLuint vertex_buffer, GLuint index_buffer, GLuint vertex_array, uint index_buffer_size)
    :vertex_buffer(vertex_buffer), index_buffer(index_buffer), vertex_array(vertex_array), index_buffer_size(index_buffer_size)
{
}

uint BindedVertexInfo::get_index_buffer_size()
{
    return this->index_buffer_size;
}

GLuint BindedVertexInfo::get_vertex_array()
{
    return this->vertex_array;
}
