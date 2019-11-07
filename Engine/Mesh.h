#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include <GL/glew.h>
#include "MathGeoLib.h"

class Mesh
{
public:
	struct Vertex {
		float3 position;
		float2 tex_coords;
	};

	Mesh();
	Mesh(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices, const std::vector<unsigned int> textures);

	~Mesh();

	void Render(const GLuint shader_program);

private:
	void setupMesh();

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> textures;

private:
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
};
