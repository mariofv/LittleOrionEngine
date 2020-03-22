#include "Mesh.h"


Mesh::Mesh(std::vector<Vertex> && vertices, std::vector<uint32_t> && indices, std::string mesh_file_path) : vertices(vertices),
indices(indices),
Resource(0, mesh_file_path)
{
	LoadInMemory();
	
}
Mesh::Mesh(std::string mesh_file_path) :
Resource(0, mesh_file_path)
{
}
Mesh::~Mesh() {
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

GLuint Mesh::GetVBO() const
{
	return vbo;
}

int Mesh::GetNumTriangles() const
{
	return indices.size() / 3;
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

	// VERTEX TEXTURE COORDS
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, tex_coords));

	// VERTEX NORMALS
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, normals));

	// VERTEX TANGENT
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, tangent));

	
	// VERTEX JOINTS
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_INT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, joints));

	// VERTEX WEIGHTS
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, weights));

	// VERTEX NUM WEIGHTS
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, num_joints));

	glBindVertexArray(0);
}