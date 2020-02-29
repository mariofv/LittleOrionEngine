#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <vector>
#include "MathGeoLib.h"

#include <GL/glew.h>
#include "Resource.h"
#include <ResourceManagement/Loaders/MaterialLoader.h>

class Material : public Resource
{
public:
	ComponentMaterial();
	~ComponentMaterial();

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void SetMaterialTexture(size_t type, const std::shared_ptr<Texture> & new_texture);
	const std::shared_ptr<Texture>& GetMaterialTexture(size_t type) const;
	
	void RemoveMaterialTexture(size_t type);

public:
	std::string shader_program = "Blinn phong";

private:
	int index = 0;
	std::vector<std::shared_ptr<Texture>> textures;

	float diffuse_color[4] = { 1.0f, 1.0f,1.0f,1.0f };
	float emissive_color[4] = { 0.0f, 0.0f, 0.0f , 1.0f };
	float specular_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float k_diffuse = 1.0f;
	float k_specular = 1.0f;
	float k_ambient = 1.0f;
	float shininess = 1.0f;

	bool show_checkerboard_texture = false;
};

namespace Loader
{
	template<>
	static std::shared_ptr<Material> Load(const std::string& uid) {
		return MaterialLoader::Load(uid);
	}
}
#endif // !_MESH_H_

