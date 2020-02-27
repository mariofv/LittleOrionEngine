#ifndef _COMPONENTMESHRENDERER_H_
#define _COMPONENTMESHRENDERER_H_

#include "Component.h"
#include "ResourceManagement/Resources/Mesh.h"
#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer(const std::shared_ptr<Mesh> & mesh_to_render);
	ComponentMeshRenderer(const std::shared_ptr<Mesh> & mesh_to_render, GameObject * owner);
	ComponentMeshRenderer();

	~ComponentMeshRenderer();

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

#endif //_COMPONENTMESHRENDERER_H_