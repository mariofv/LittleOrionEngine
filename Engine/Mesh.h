#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "MathGeoLib.h"

#include <GL/glew.h>

class Mesh
{
public:
	struct Vertex {
		float3 position;
		float2 tex_coords;
		float3 normals;
	};
	Mesh(std::vector<Vertex> && vertices, std::vector<uint32_t> && indices);
	Mesh(std::vector<Vertex> && vertices, std::vector<uint32_t> && indices, std::string mesh_file_path);
	Mesh(std::vector<Vertex> && vertices, std::vector<uint32_t> && indices, std::vector<std::string> && meshes_textures_path, std::string mesh_file_path);
	~Mesh();

	GLuint GetVAO() const;
private:
	void InitMesh();
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Triangle> triangles;
	std::string mesh_file_path;
	std::vector<std::string> meshes_textures_path;

private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
};
#endif // !_MESH_H_

