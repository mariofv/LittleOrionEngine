#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Resource.h"

#include "Helper/Config.h"
#include "ResourceManagement/Loaders/MaterialLoader.h"
#include "Texture.h"

#include <vector>
#include <GL/glew.h>
#include <MathGeoLib.h>

class Material : public Resource
{
public:
	enum class MaterialType
	{
		MATERIAL_OPAQUE,
		MATERIAL_TRANSPARENT,
		UNKNOWN
	};

	enum MaterialTextureType
	{
		DIFFUSE,
		SPECULAR,
		EMISSIVE,
		OCCLUSION,
		NORMAL,
		UNKNOWN
	};

	Material(uint32_t UUID, std::string material_file_path);
	~Material() = default;

	void Save(Config& config) const;
	void Load(const Config& config);

	void SetMaterialTexture(MaterialTextureType type, const std::shared_ptr<Texture> & new_texture);
	const std::shared_ptr<Texture>& GetMaterialTexture(MaterialTextureType type) const;

	void RemoveMaterialTexture(MaterialTextureType type);

	void LoadInMemory() override {}

	void ChangeTypeOfMaterial(const MaterialType new_material_type);
	const char* GetMaterialTypeName(const MaterialType material_type);


public:
	static const size_t MAX_MATERIAL_TEXTURE_TYPES = static_cast<size_t>(MaterialTextureType::UNKNOWN);
	static const size_t MAX_MATERIAL_TYPES = static_cast<size_t>(MaterialType::UNKNOWN);

	MaterialType material_type = MaterialType::MATERIAL_OPAQUE;

	std::string shader_program = "Blinn phong";
	std::vector<std::shared_ptr<Texture>> textures;

	float diffuse_color[4] = { 1.0f, 1.0f,1.0f,1.0f };
	float emissive_color[4] = { 0.0f, 0.0f, 0.0f , 1.0f };
	float specular_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float k_diffuse = 1.0f;
	float k_specular = 1.0f;
	float k_ambient = 1.0f;

	float2 coords = float2(1.0f, 1.0f);

	float transparency = 0.5F;
	float roughness = 0.5f;
	float metalness = 0.04f;

	float tiling_x = 1.0f;
	float tiling_y = 1.0f;

	//This variable will allow the shader to use the normal maps if there is any assigned to the model material
	bool use_normal_map = false;

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

