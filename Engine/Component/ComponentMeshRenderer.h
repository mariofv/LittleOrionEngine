#ifndef _COMPONENTMESHRENDERER_H_
#define _COMPONENTMESHRENDERER_H_

#include "Component.h"
#include "ResourceManagement/Resources/Mesh.h"
#include "ResourceManagement/Resources/Material.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer();
	ComponentMeshRenderer(GameObject * owner);
	~ComponentMeshRenderer() = default;

	ComponentMeshRenderer(const ComponentMeshRenderer& component_to_copy) = default;
	ComponentMeshRenderer(ComponentMeshRenderer&& component_to_move) = default;

	ComponentMeshRenderer& operator=(const ComponentMeshRenderer& component_to_copy) = default;
	ComponentMeshRenderer& operator=(ComponentMeshRenderer&& component_to_move) = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void Delete() override;

	void Render() const;
	void RenderModel() const;
	void RenderMaterial(GLuint shader_program) const;

	void SetMesh(uint32_t mesh_uuid);
	void SetMaterial(uint32_t material_uuid);

private:
	void AddDiffuseUniforms(unsigned int shader_program) const;
	void AddEmissiveUniforms(unsigned int shader_program) const;
	void AddSpecularUniforms(unsigned int shader_program) const;
	void AddAmbientOclusionUniforms(unsigned int shader_program) const;
	void BindTexture(Material::MaterialTextureType id) const;

public:
	uint32_t mesh_uuid;
	uint32_t material_uuid;

	std::shared_ptr<Mesh> mesh_to_render;
	std::shared_ptr<Material> material_to_render;

private:

	friend class PanelComponent;
};

#endif //_COMPONENTMESHRENDERER_H_