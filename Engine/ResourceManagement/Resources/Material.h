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
		MATERIAL_LIQUID,
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
		LIQUID,
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

	void UpdateLiquidProperties();
	unsigned int GetShaderVariation() const;


public:
	static const size_t MAX_MATERIAL_TEXTURE_TYPES = static_cast<size_t>(MaterialTextureType::UNKNOWN);
	static const size_t MAX_MATERIAL_TYPES = static_cast<size_t>(MaterialType::UNKNOWN);

	MaterialType material_type = MaterialType::MATERIAL_OPAQUE;

	std::string shader_program = "Blinn phong";
	
	std::vector<uint32_t> textures_uuid;
	std::vector<std::shared_ptr<Texture>> textures;

	float diffuse_color[4] = { 1.0f, 1.0f,1.0f,1.0f };
	float emissive_color[4] = { 1.0f, 1.0f, 1.0f , 1.0f };
	float specular_color[4] = { 0.025f, 0.025f, 0.025f, 0.025f };
	float smoothness = 1.0F;

	float2 coords = float2(1.0f, 1.0f);

	float transparency = 0.5F;

	float tiling_x = 1.0f;
	float tiling_y = 1.0f;

	//liquid material
	float speed_tiling_x = 1.0F;
	float speed_tiling_y = 1.0F;
	float tiling_liquid_x_x = 1.F;
	float tiling_liquid_x_y = 1.F;
	float tiling_liquid_y_x = -1.F;
	float tiling_liquid_y_y = -1.F;
	bool use_liquid_map = false;

	bool show_checkerboard_texture = false;
	bool use_shadow_map = true;
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

