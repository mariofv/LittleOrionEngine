#ifndef _COMPONENTMATERIAL_H_
#define _COMPONENTMATERIAL_H_

#include "Component.h"
#include "Main/Globals.h"
#include "UI/Panel/InspectorSubpanel/PanelComponent.h"
#include "ResourceManagement/Resources/Texture.h"

#include <GL/glew.h>
#include <memory>


class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	ComponentMaterial(GameObject * owner);
	~ComponentMaterial();

	//Copy and move
	ComponentMaterial(const ComponentMaterial& component_to_copy) = default;
	ComponentMaterial(ComponentMaterial&& component_to_move) = default;

	virtual ComponentMaterial & operator=(const ComponentMaterial & component_to_copy) = default;
	virtual ComponentMaterial & operator=(ComponentMaterial && component_to_move) = default;

	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void Render(unsigned int shader_program) const;

	void SetMaterialTexture(size_t type, const std::shared_ptr<Texture> & new_texture);
	const std::shared_ptr<Texture>& GetMaterialTexture(size_t type) const;
	void RemoveMaterialTexture(size_t type);
	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component * component_to_copy) const override { *static_cast<ComponentMaterial*>(component_to_copy) = *this; };

private:
	void AddDiffuseUniforms(unsigned int shader_program) const;
	void AddEmissiveUniforms(unsigned int shader_program) const;
	void AddSpecularUniforms(unsigned int shader_program) const;
	void AddAmbientOclusionUniforms(unsigned int shader_program) const;
	void BindTexture(Texture::TextureType id) const;

private:
	ComponentType type = ComponentType::MATERIAL;

	std::vector<std::shared_ptr<Texture>> textures;

	float diffuse_color[4] = { 1.0f, 1.0f,1.0f,1.0f };
	float emissive_color[4] = { 0.0f, 0.0f, 0.0f , 1.0f};
	float specular_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float k_diffuse = 1.0f;
	float k_specular = 1.0f;
	float k_ambient = 1.0f;
	float shininess = 1.0f;

	bool show_checkerboard_texture = false;

	friend class PanelComponent;
};

#endif //_COMPONENTMATERIAL_H_