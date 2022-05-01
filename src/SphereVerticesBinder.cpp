#include "SphereVerticesBinder.h"

BindedVertexInfo* SphereVerticesBinder::bind()
{
	GLuint vertex_array = 0;
	GLuint vertex_buffer = 0;
	GLuint index_buffer = 0;
	uint index_buffer_size = 0;

	std::vector<vertex>	vertex_list;

	const float radius = 1.0f;
	const int theta_count = 50;
	const int pi_count = 100;

	const float top_vertex_theta = 0.0f;
	const float top_vertex_pi = 0.0f;

	vertex top_vertex = {
		SphereVerticesBinder::get_vertex_position(radius, top_vertex_theta, top_vertex_pi),
		SphereVerticesBinder::get_vertex_normal_vector(top_vertex_theta, top_vertex_pi),
		SphereVerticesBinder::get_texture_coordinate(top_vertex_theta, top_vertex_pi),
	};

	vertex_list.push_back(top_vertex);

	for (int i = 1; i < theta_count; i++)
	{
		float theta = (PI / theta_count) * i;

		for (int j = 0; j < pi_count; j++)
		{
			float pi = (2 * PI / pi_count) * j;

			vertex bottom_vertex = {
				SphereVerticesBinder::get_vertex_position(radius, theta, pi),
				SphereVerticesBinder::get_vertex_normal_vector(theta, pi),
				SphereVerticesBinder::get_texture_coordinate(theta, pi),
			};

			vertex_list.push_back(bottom_vertex);
		}
	}

	const float bottom_vertex_theta = PI;
	const float bottom_vertex_pi = 0.0f;

	vertex bottom_vertex = {
		SphereVerticesBinder::get_vertex_position(radius, bottom_vertex_theta, bottom_vertex_pi),
		SphereVerticesBinder::get_vertex_normal_vector(bottom_vertex_theta, bottom_vertex_pi),
		SphereVerticesBinder::get_texture_coordinate(bottom_vertex_theta, bottom_vertex_pi),
	};

	vertex_list.push_back(bottom_vertex);

	// create a vertex buffer
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertex_list.size(), &vertex_list[0], GL_STATIC_DRAW);


	std::vector<uint> index_list;

	for (int i = 1; i < theta_count; i++)
	{
		int start_index = 1 + pi_count * (i - 1);
		int last_index = start_index + pi_count - 1;

		if (i == 1)
		{
			for (int j = 0; j < pi_count - 1; j++)
			{
				index_list.push_back(start_index - 1);
				index_list.push_back(start_index + j);
				index_list.push_back(start_index + j + 1);
			}

			index_list.push_back(last_index - pi_count);
			index_list.push_back(last_index);
			index_list.push_back(start_index);
		}
		else
		{
			for (int j = 0; j < pi_count - 1; j++)
			{
				index_list.push_back(start_index + j - pi_count);
				index_list.push_back(start_index + j);
				index_list.push_back(start_index + j + 1);
			}
			index_list.push_back(last_index - pi_count);
			index_list.push_back(last_index);
			index_list.push_back(start_index);
		}

		if (i == theta_count - 1)
		{
			for (int j = 0; j < pi_count - 1; j++)
			{
				index_list.push_back(start_index + j);
				index_list.push_back(last_index + 1);
				index_list.push_back(start_index + j + 1);
			}

			index_list.push_back(last_index);
			index_list.push_back(last_index + 1);
			index_list.push_back(start_index);
		}
		else
		{
			for (int j = 0; j < pi_count - 1; j++)
			{
				index_list.push_back(start_index + j);
				index_list.push_back(start_index + j + 1 + pi_count);
				index_list.push_back(start_index + j + 1);
			}
			index_list.push_back(last_index);
			index_list.push_back(last_index + 1);
			index_list.push_back(start_index);
		}
	}

	index_buffer_size = index_list.size();

	// create a index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * index_list.size(), &index_list[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); }

	return new BindedVertexInfo(vertex_buffer, index_buffer, vertex_array, index_buffer_size);
}

vec3 SphereVerticesBinder::get_vertex_position(float r, float theta, float pi)
{
	float sin_theta = sin(theta);
	float cos_theta = cos(theta);
	float sin_pi = sin(pi);
	float cos_pi = cos(pi);

	return vec3(r * sin_theta * cos_pi, r * sin_theta * sin_pi, cos_theta);
}

vec3 SphereVerticesBinder::get_vertex_normal_vector(float theta, float pi)
{
	float sin_theta = sin(theta);
	float cos_theta = cos(theta);
	float sin_pi = sin(pi);
	float cos_pi = cos(pi);

	return vec3(sin_theta * cos_pi, sin_theta * sin_pi, cos_theta);
}

vec2 SphereVerticesBinder::get_texture_coordinate(float theta, float pi)
{
	return vec2(pi / (2 * PI), 1 - theta / PI);
}
