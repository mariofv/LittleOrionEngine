#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> && vertices, std::vector<UINT64> && indices) : 
	vertices(vertices), 
	indices(indices)
{
	num_vertices = vertices.size();
	num_triangles = num_vertices / 3;
}

Mesh::~Mesh() {
	vertices.clear();
	indices.clear();
}