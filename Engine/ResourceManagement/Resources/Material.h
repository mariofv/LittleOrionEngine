#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Resource.h"

#include "Helper/Config.h"
#include "ResourceManagement/Manager/MaterialManager.h"
#include "Texture.h"

#include <GL/glew.h>
#include <MathGeoLib.h>
#include <vector>

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
		LIGHTMAP,
		UNKNOWN
	};

	Material() = default;
	Material(uint32_t uuid);
	~Material() = default;

	void Save(Config& config) const;
	void Load(const Config& config);

	void SetMaterialTexture(MaterialTextureType type, uint32_t texture_id);
	const std::shared_ptr<Texture>& GetMaterialTexture(MaterialTextureType type) const;

	void RemoveMaterialTexture(MaterialTextureType type);

	void ChangeTypeOfMaterial(const MaterialType new_material_type);
	static std::string GetMaterialTypeName(const MaterialType material_type);


public:
	static const size_t MAX_MATERIAL_TEXTURE_TYPES = static_cast<size_t>(MaterialTextureType::UNKNOWN);
	static const size_t MAX_MATERIAL_TYPES = static_cast<size_t>(MaterialType::UNKNOWN);

	MaterialType material_type = MaterialType::MATERIAL_OPAQUE;

	std::string shader_program = "Blinn phong";
	
	std::vector<uint32_t> textures_uuid;
	std::vector<std::shared_ptr<Texture>> textures;

	float diffuse_color[4] = { 1.0f, 1.0f,1.0f,1.0f };
	float emissive_color[4] = { 0.0f, 0.0f, 0.0f , 1.0f };
	float specular_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float k_diffuse = 1.0f;
	float k_specular = 1.0f;
	float k_ambient = 1.0f;
	float2 coords = float2(1.0f, 1.0f);

	float transparency = 0.5F;
//	float roughness = 0.5f;
//	float metalness = 0.04f;

	float tiling_x = 1.0f;
	float tiling_y = 1.0f;

	//This variable will allow the shader to use the normal maps if there is any assigned to the model material
	bool use_normal_map = false;

	bool show_checkerboard_texture = false;
};

namespace ResourceManagement
{
	template<>
	static FileData Binarize<Material>(Resource* material)
	{
		return MaterialManager::Binarize(static_cast<Material*>(material));
	};

	template<>
	static std::shared_ptr<Material> Load(uint32_t uuid, const FileData& resource_data)
	{
		return MaterialManager::Load(uuid, resource_data);
	}

	template<>
	static FileData Create<Material>()
	{
		return MaterialManager::Create();
	};
}

#endif // !_MATERIAL_H_

