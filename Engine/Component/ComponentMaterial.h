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
	enum class MaterialType
	{
		MATERIALOPAQUE,
		MATERIALTRANSPARENT,
		NUM_OF_ITEMS
	};

	ComponentMaterial();
	ComponentMaterial(GameObject * owner);
	~ComponentMaterial();

	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void Render(unsigned int shader_program) const;

	void SetMaterialTexture(size_t type, const std::shared_ptr<Texture> & new_texture);
	const std::shared_ptr<Texture>& GetMaterialTexture(size_t type) const;
	const char* TypeOfMaterial(const MaterialType material_type);
	void ChangeTypeOfMaterial(const MaterialType new_material_type);
	void RemoveMaterialTexture(size_t type);

private:
	void AddDiffuseUniforms(unsigned int shader_program) const;
	void AddEmissiveUniforms(unsigned int shader_program) const;
	void AddSpecularUniforms(unsigned int shader_program) const;
	void AddAmbientOclusionUniforms(unsigned int shader_program) const;
	void AddNormalUniforms(unsigned int shader_program) const;
	void AddExtraUniforms(unsigned int shader_program) const;
	void BindTexture(Texture::TextureType id) const;

	bool BindTextureNormal(Texture::TextureType id) const;

public:
	std::string shader_program = "Blinn phong";

private:
	ComponentType type = ComponentType::MATERIAL;

	int index = 0;
	std::vector<std::shared_ptr<Texture>> textures;

	float diffuse_color[4] = { 1.0f, 1.0f,1.0f,1.0f };
	float emissive_color[4] = { 0.0f, 0.0f, 0.0f , 1.0f};
	float specular_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float k_diffuse = 1.0f;
	float k_specular = 1.0f;
	float k_ambient = 1.0f;
	float shininess = 1.0f; //TODO-> erase shininess from all the code. 
	float k_normal = 1.0f;
	float alpha_blending = 1.0F;
	float roughness = 0.5f;
	float metalness = 0.04f;

	//Material type
	MaterialType material_type = MaterialType::MATERIALOPAQUE;

	bool show_checkerboard_texture = false;

	friend class PanelComponent;
};

#endif //_COMPONENTMATERIAL_H_