#ifndef _COMPONENTMATERIAL_H_
#define _COMPONENTMATERIAL_H_

#include "Component.h"
#include "UI/ComponentsUI.h"
#include "Globals.h"
#include "Texture.h"

#include <GL/glew.h>
#include <memory>


class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	ComponentMaterial(GameObject * owner);
	~ComponentMaterial();

	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void Render(unsigned int shader_program) const;

	void SetMaterialTexture(size_t type, const std::shared_ptr<Texture> & new_texture);
	const std::shared_ptr<Texture>& GetMaterialTexture(size_t type) const;
	void RemoveMaterialTexture(size_t type);

	void ShowComponentWindow() override;

private:
	void AddDiffuseUniforms(unsigned int shader_program) const;
	void AddEmissiveUniforms(unsigned int shader_program) const;
	void AddSpecularUniforms(unsigned int shader_program) const;
	void AddAmbientOclusionUniforms(unsigned int shader_program) const;
	void BindTexture(Texture::TextureType id) const;
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
	float shininess = 1.0f;

	bool show_checkerboard_texture = false;

	friend void ComponentsUI::ShowComponentMaterialWindow(ComponentMaterial *material);
	friend class ComponentsUI;
};

#endif //_COMPONENTMATERIAL_H_