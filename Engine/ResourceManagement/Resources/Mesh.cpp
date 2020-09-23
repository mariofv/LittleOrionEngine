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

	// VERTEX POSITION
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)0);

	// VERTEX UV 0
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, tex_coords[0]));

	// VERTEX NORMALS
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, normals));

	// VERTEX TANGENT
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, tangent));

	// VERTEX JOINTS
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, joints));

	// VERTEX WEIGHTS
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, weights));

	// NUM JOINTS
	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, num_joints));

	// VERTEX UV 1
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, tex_coords[1]));
	glBindVertexArray(0);


	glBindVertexArray(0);

	initialized = true;
}

