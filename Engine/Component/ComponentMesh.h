#ifndef _COMPONENTMESH_H_
#define _COMPONENTMESH_H_

#include "Component.h"

#include "Mesh.h"

class ComponentsUI;
class ComponentMesh : public Component
{
public:
	ComponentMesh(const std::shared_ptr<Mesh> & mesh_to_render);
	ComponentMesh(const std::shared_ptr<Mesh> & mesh_to_render, GameObject * owner);
	ComponentMesh();

	~ComponentMesh();

	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void Render() const;

	void SetMesh(const std::shared_ptr<Mesh> & mesh_to_render);

	void ShowComponentWindow() override;

public:
	std::shared_ptr<Mesh> mesh_to_render;
	unsigned int shader_program = 0;

private:
	friend class ComponentsUI;
};

#endif //_COMPONENTMESH_H_