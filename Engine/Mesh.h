#ifndef _MESH_H_
#define _MESH_H_

#include "Module.h"
#include "Globals.h"

#include <GL/glew.h>
#include "MathGeoLib.h"

#include <vector>

class Mesh
{
public:
	struct Vertex {
		float3 position;
		float2 tex_coords;
	};

	Mesh();
	Mesh(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices, const unsigned int texture_index);

	~Mesh();

	void Render(const GLuint shader_program, const GLuint texture);

private:
	void setupMesh();

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int material_index;

private:
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
};

#endif //_MESH_H_