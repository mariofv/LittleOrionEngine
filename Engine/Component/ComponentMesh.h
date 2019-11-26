#ifndef _COMPONENTMESH_H_
#define _COMPONENTMESH_H_

#include "Component.h"

#include <GL/glew.h>
#include "MathGeoLib.h"

#include <vector>

class ComponentMesh : public Component
{
public:
	struct Vertex {
		float3 position;
		float2 tex_coords;
	};

	ComponentMesh() = default;
	~ComponentMesh();

	void Enable() override;
	void Disable() override;
	void Update() override;

	ComponentType GetType() const override;

	void LoadMesh(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices, const unsigned int texture_index);
	void Render(const GLuint shader_program, const GLuint texture) const;

	void ShowComponentWindow() override;

private:
	void SetupMesh();

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int material_index = -1;

private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;

	int num_triangles = 0;
	int num_vertices = 0;
};

#endif //_COMPONENTMESH_H_