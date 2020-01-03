#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> && vertices, std::vector<uint32_t> && indices) : 
	vertices(vertices), 
	indices(indices)
{
	InitMesh();
}

Mesh::Mesh(std::vector<Vertex> && vertices, std::vector<uint32_t> && indices, std::string mesh_file_path) : vertices(vertices),
indices(indices),
mesh_file_path(mesh_file_path)
{
	InitMesh();
}
Mesh::Mesh(std::vector<Vertex> && vertices, std::vector<uint32_t> && indices, std::vector<std::string> && meshes_textures_path ,std::string mesh_file_path) : vertices(vertices),
indices(indices),
meshes_textures_path(meshes_textures_path),
mesh_file_path(mesh_file_path)
{
	InitMesh();
	
}
Mesh::~Mesh() {
	vertices.clear();
	indices.clear();
}

void Mesh::InitMesh()
{
	for (size_t i = 0; i <indices.size(); i += 3)
	{
		float3 first_point = vertices[indices[i]].position;
		float3 second_point =vertices[indices[i + 1]].position;
		float3 third_point = vertices[indices[i + 2]].position;
		triangles.push_back(Triangle(first_point, second_point, third_point));
	}
	num_vertices = vertices.size();
	num_triangles = triangles.size();
}