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
	enum MaterialTextureType
	{
		DIFFUSE,
		SPECULAR,
		EMISSIVE,
		OCCLUSION,
		UNKNOWN
	};

	Material() = default;
	Material(Metafile* resource_metafile);
	~Material() = default;

	void Save(Config& config) const;
	void Load(const Config& config);

	void SetMaterialTexture(MaterialTextureType type, uint32_t texture_id);
	const std::shared_ptr<Texture>& GetMaterialTexture(MaterialTextureType type) const;

	void RemoveMaterialTexture(MaterialTextureType type);

public:
	static const size_t MAX_MATERIAL_TEXTURE_TYPES = static_cast<size_t>(MaterialTextureType::UNKNOWN);
	std::string shader_program = "Blinn phong";
	
	std::vector<uint32_t> textures_uuid;
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
	static std::shared_ptr<Material> Load(Metafile* metafile, const FileData& resource_data)
	{
		return MaterialManager::Load(metafile, resource_data);
	}
}

#endif // !_MESH_H_

