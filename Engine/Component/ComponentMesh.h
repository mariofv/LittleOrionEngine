#ifndef _COMPONENTMESH_H_
#define _COMPONENTMESH_H_

#include "Component.h"
#include "UI/ComponentsUI.h"

#include <GL/glew.h>
#include "Mesh.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh(std::shared_ptr<Mesh> mesh_to_render);
	ComponentMesh(std::shared_ptr<Mesh> mesh_to_render, GameObject * owner);
	ComponentMesh();

	~ComponentMesh();

	void Delete() override;
	void Render() const;

	void SetMesh(std::shared_ptr<Mesh> mesh_to_render);

	void ShowComponentWindow() override;

private:
	void SetupMesh();

public:
	unsigned int material_index = -1;
	std::shared_ptr<Mesh> mesh_to_render;

private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;

	friend void ComponentsUI::ShowComponentMeshWindow(ComponentMesh *mesh);
};

#endif //_COMPONENTMESH_H_