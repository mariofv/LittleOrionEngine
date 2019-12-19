#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "MathGeoLib.h"

class Mesh
{
public:
	struct Vertex {
		float3 position;
		float2 tex_coords;
	};
	Mesh(std::vector<Vertex> && vertices, std::vector<UINT64> && indices);
	Mesh(std::vector<Vertex> && vertices, std::vector<UINT64> && indices, std::string mesh_file_path);
	~Mesh();

	std::vector<Vertex> vertices;
	std::vector<UINT64> indices;
	std::string mesh_file_path;
	int num_triangles = 0;
	int num_vertices = 0;
};
#endif // !_MESH_H_

