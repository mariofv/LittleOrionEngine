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
	void RenderModel() const;

	void SetMesh(const std::shared_ptr<Mesh> & mesh_to_render);
	bool operator <(const ComponentMesh & mesh_to_compare) const;

	void ShowComponentWindow() override;
	unsigned int shader_program = 0;
	std::shared_ptr<Mesh> mesh_to_render;
private:
	friend class ComponentsUI;
};

#endif //_COMPONENTMESH_H_