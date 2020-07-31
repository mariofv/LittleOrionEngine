#include "Mesh.h"

#include "ResourceManagement/Metafile/Metafile.h"

Mesh::Mesh(uint32_t uuid, std::vector<Vertex> && vertices, std::vector<uint32_t> && indices, bool async)
	: vertices(vertices)
	, indices(indices)
	, Resource(uuid)
{
	if(!async)
	{
		LoadInMemory();
	}
}

Mesh::~Mesh() 
{
	if (vbo != 0)
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteVertexArrays(1, &vao);
	}
}

GLuint Mesh::GetVAO() const
{
	return vao;
}

GLuint Mesh::GetEBO() const {
	return ebo;
}

int Mesh::GetNumTriangles() const
{
	return indices.size() / 3;
}

int Mesh::GetNumVerts() const
{
	return vertices.size();
}

std::vector<Triangle> Mesh::GetTriangles() const
{
	std::vector<Triangle> triangles;
	triangles.reserve(indices.size()/3);
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		float3 first_point = vertices[indices[i]].position;
		float3 second_point = vertices[indices[i + 1]].position;
		float3 third_point = vertices[indices[i + 2]].position;
		triangles.emplace_back(Triangle(first_point, second_point, third_point));
	}
	return triangles;
}

void Mesh::LoadInMemory()
{

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Mesh::Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

	// VERTEX POSITION -- vertex_position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)0);

	// VERTEX TEXTURE COORDS -- vertex_uv0
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, tex_coords[0]));

	// vertex_uv1
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, tex_coords[1]));

	// VERTEX NORMALS -- vertex_normal
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, normals));

	// VERTEX TANGENT -- vertex_tangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, tangent));

	// VERTEX JOINTS -- vertex_joints
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_UNSIGNED_INT, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, joints));

	// VERTEX NUM JOINTS -- vertex_num_joints
	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, num_joints));

	// VERTEX WEIGHTS -- vertex_weights
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, weights));


	glBindVertexArray(0);

	initialized = true;
}

