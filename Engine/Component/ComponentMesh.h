#ifndef _COMPONENTMESH_H_
#define _COMPONENTMESH_H_

#include "Component.h"
#include "ResourceManagement/Resources/Mesh.h"
#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

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

public:
	std::shared_ptr<Mesh> mesh_to_render;

private:
	friend class PanelComponent;
};

#endif //_COMPONENTMESH_H_