#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Resource.h"
#include "Texture.h"
#include "ResourceManagement/Loaders/MaterialLoader.h"

#include <vector>
#include <GL/glew.h>
#include <MathGeoLib.h>

class Material : public Resource
{
public:
	enum MaterialTextureType
	{
		DIFFUSE,
		SPECULAR,
		EMISSIVE,
		OCCLUSION,
		UNKNOWN
	};

	Material(std::string material_file_path);
	~Material();

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void SetMaterialTexture(MaterialTextureType type, const std::shared_ptr<Texture> & new_texture);
	const std::shared_ptr<Texture>& GetMaterialTexture(MaterialTextureType type) const;
	
	void RemoveMaterialTexture(MaterialTextureType type);

	void LoadInMemory() override {}

public:
	std::string shader_program = "Blinn phong";
	static const size_t MAX_MATERIAL_TEXTURE_TYPES = static_cast<size_t>(MaterialTextureType::UNKNOWN);

private:
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

