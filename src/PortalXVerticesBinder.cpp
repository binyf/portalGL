#include "PortalXVerticesBinder.h"

BindedVertexInfo* PortalXVerticesBinder::bind()
{
	GLuint vertex_array = 0;
	GLuint vertex_buffer = 0;
	GLuint index_buffer = 0;
	uint index_buffer_size = 0;

	uint number_of_triangles_in_circle = 64;

	std::vector<vec3> cube_vertices{ 
		vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, -0.5f, 0.5f), vec3(-0.5f, -0.5f, 0.5f), vec3(-0.5f, 0.5f, 0.5f),
		vec3(0.5f, 0.5f, -0.5f), vec3(0.5f, -0.5f, -0.5f), vec3(-0.5f, -0.5f, -0.5f), vec3(-0.5f, 0.5f, -0.5f),
	};

	std::vector<vertex> vertices;
	vertices.push_back({ cube_vertices[0], vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[1], vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[2], vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[3], vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) });

	vertices.push_back({ cube_vertices[4], vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[0], vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[3], vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[7], vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f) });

	vertices.push_back({ cube_vertices[0], vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f) });
	vertices.push_back({ cube_vertices[4], vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f) });
	vertices.push_back({ cube_vertices[5], vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[1], vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f) });

	vertices.push_back({ cube_vertices[1], vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[5], vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[6], vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[2], vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f) });

	vertices.push_back({ cube_vertices[2], vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f) });
	vertices.push_back({ cube_vertices[6], vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f) });
	vertices.push_back({ cube_vertices[7], vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[3], vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f) });

	vertices.push_back({ cube_vertices[4], vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[7], vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[6], vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f) });
	vertices.push_back({ cube_vertices[5], vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f) });

	std::vector<uint> indices;
	for (int k = 0; k < 6; k++) {
		indices.push_back(k * 4 + 0);
		indices.push_back(k * 4 + 2);
		indices.push_back(k * 4 + 1);

		indices.push_back(k * 4 + 0);
		indices.push_back(k * 4 + 3);
		indices.push_back(k * 4 + 2);
	}

	index_buffer_size = indices.size();

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); }

	return new BindedVertexInfo(vertex_buffer, index_buffer, vertex_array, index_buffer_size);
}
