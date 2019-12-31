#ifndef _COMPONENTMESH_H_
#define _COMPONENTMESH_H_

#include "Component.h"

#include <GL/glew.h>
#include "Mesh.h"

class ComponentsUI;
class ComponentMesh : public Component
{
public:
	ComponentMesh(std::shared_ptr<Mesh> mesh_to_render);
	ComponentMesh(std::shared_ptr<Mesh> mesh_to_render, GameObject * owner);
	ComponentMesh();

	~ComponentMesh();

	void Delete() override;

	void Save(Config& config) const;
	void Load(const Config& config);

	void Render() const;

	void SetMesh(std::shared_ptr<Mesh> mesh_to_render);

	void ShowComponentWindow() override;

private:
	void SetupMesh();

public:
	std::shared_ptr<Mesh> mesh_to_render;
	unsigned int shader_program = 0;

private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	friend class ComponentsUI;
};

#endif //_COMPONENTMESH_H_