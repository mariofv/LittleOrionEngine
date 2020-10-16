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
		MATERIAL_OPAQUE = 0,
		MATERIAL_TRANSPARENT = 1,
		MATERIAL_DISSOLVING = 2,
		MATERIAL_LIQUID = 3,
		UNKNOWN = 4
	};

	enum MaterialTextureType
	{
		DIFFUSE = 0,
		SPECULAR = 1,
		EMISSIVE = 2,
		OCCLUSION = 3,
		NORMAL = 4,
		LIGHTMAP = 5,
		LIQUID = 6,
		DISSOLVED_DIFFUSE = 7,
		DISSOLVED_EMISSIVE = 8,
		NOISE = 9,
		UNKNOWN = 10
	};

	Material();
	Material(uint32_t uuid);
	~Material() = default;

	void Save(Config& config) const;
	void Load(const Config& config);

	void SetMaterialTexture(MaterialTextureType type, uint32_t texture_id);
	const std::shared_ptr<Texture>& GetMaterialTexture(MaterialTextureType type) const;
	bool UseLightmap() const;

	//Asyncronous loading
	void LoadResource(uint32_t uuid, unsigned texture_type);
	void InitResource(uint32_t uuid, unsigned texture_type);

	void RemoveMaterialTexture(MaterialTextureType type);

	void ChangeTypeOfMaterial(const MaterialType new_material_type);
	static std::string GetMaterialTypeName(const MaterialType material_type);

	void UpdateLiquidProperties();
	unsigned int GetShaderVariation() const;

	ENGINE_API void SetDissolveProgress(float progress);
	ENGINE_API void SetFinalAddedColor(const float4& final_added_color);

	ENGINE_API std::shared_ptr<Material> GetInstance();

public:
	static const size_t MAX_MATERIAL_TEXTURE_TYPES = static_cast<size_t>(MaterialTextureType::UNKNOWN);
	static const size_t MAX_MATERIAL_TYPES = static_cast<size_t>(MaterialType::UNKNOWN);

	MaterialType material_type = MaterialType::MATERIAL_OPAQUE;

	std::string shader_program = "Blinn phong";
	
	std::vector<uint32_t> textures_uuid;
	std::vector<std::shared_ptr<Texture>> textures;


	float diffuse_color[4] = { 1.0f, 1.0f,1.0f,1.0f };
	float emissive_color[4] = { 1.0f, 1.0f, 1.0f , 1.0f };
	float emissive_intensity = 1.f;
	float specular_color[4] = { 0.025f, 0.025f, 0.025f, 0.025f };
	float smoothness = 1.0F;

	float transparency = 1.0F;
	float reflection_strength = 0.0f;
	float2 tiling = float2::one;

	//liquid material
	float2 liquid_tiling_speed = float2::one;
	float2 liquid_horizontal_normals_tiling = float2::one;
	float2 liquid_vertical_normals_tiling = -float2::one;

	float dissolve_progress = 0.f;

	float4 final_added_color = float4::zero;

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
	static std::shared_ptr<Material> Load(uint32_t uuid, const FileData& resource_data, bool async)
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

