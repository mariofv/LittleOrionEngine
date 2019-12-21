#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> && vertices, std::vector<uint32_t> && indices) : 
	vertices(vertices), 
	indices(indices)
{
	num_vertices = vertices.size();
	num_triangles = num_vertices / 3;
}

Mesh::Mesh(std::vector<Vertex> && vertices, std::vector<uint32_t> && indices, std::string mesh_file_path) : vertices(vertices),
indices(indices),
mesh_file_path(mesh_file_path)
{
	num_vertices = vertices.size();
	num_triangles = num_vertices / 3;
}
Mesh::Mesh(std::vector<Vertex> && vertices, std::vector<uint32_t> && indices, std::vector<std::string> && meshes_textures_path ,std::string mesh_file_path) : vertices(vertices),
indices(indices),
meshes_textures_path(meshes_textures_path),
mesh_file_path(mesh_file_path)
{
	num_vertices = vertices.size();
	num_triangles = num_vertices / 3;
}
Mesh::~Mesh() {
	vertices.clear();
	indices.clear();
}