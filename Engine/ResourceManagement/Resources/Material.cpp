#include "Material.h"

#include "Helper/Config.h"
#include "Main/Application.h"

#include "Module/ModuleLight.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleTime.h"


#include "ResourceManagement/Metafile/Metafile.h"

Material::Material()
{
	textures.resize(MAX_MATERIAL_TEXTURE_TYPES);
	textures_uuid.resize(MAX_MATERIAL_TEXTURE_TYPES);
}

Material::Material(uint32_t uuid) : Resource(uuid)
{
	textures.resize(MAX_MATERIAL_TEXTURE_TYPES);
	textures_uuid.resize(MAX_MATERIAL_TEXTURE_TYPES);
}

void Material::Save(Config& config) const
{
	for (size_t i = 0; i < textures_uuid.size(); i++)
	{
		MaterialTextureType type = static_cast<MaterialTextureType>(i);

		switch (type)
		{
		case MaterialTextureType::DIFFUSE:
			config.AddUInt(textures_uuid[i], "Diffuse");
			break;

		case MaterialTextureType::SPECULAR:
			config.AddUInt(textures_uuid[i], "Specular");
			break;

		case MaterialTextureType::OCCLUSION:
			config.AddUInt(textures_uuid[i], "Occlusion");
			break;

		case MaterialTextureType::EMISSIVE:
			config.AddUInt(textures_uuid[i], "Emissive");
			break;

		case MaterialTextureType::NORMAL:
			config.AddUInt(textures_uuid[i],  "Normal");
			break;

		case MaterialTextureType::LIGHTMAP:
			config.AddUInt(textures_uuid[i], "Lightmap");
			break;

		case MaterialTextureType::LIQUID:
			config.AddUInt(textures_uuid[i], "Liquid");
			break;

		case MaterialTextureType::NOISE:
			config.AddUInt(textures_uuid[i], "Noise");
			break;

		case MaterialTextureType::DISSOLVED_DIFFUSE:
			config.AddUInt(textures_uuid[i], "Dissolved Diffuse");
			break;

		case MaterialTextureType::DISSOLVED_EMISSIVE:
			config.AddUInt(textures_uuid[i], "Dissolved Emissive");
			break;
		default:
			break;
			
		}
	}

	config.AddInt((int)material_type, "MaterialType");

	config.AddBool(show_checkerboard_texture, "Checkboard");
	config.AddString(shader_program, "ShaderProgram");

	config.AddFloat(smoothness, "Smoothness");
	config.AddFloat(emissive_intensity, "Emissive Intensity");
	config.AddFloat(transparency, "Transparency");
	config.AddFloat(reflection_strength, "Reflection");

	config.AddFloat(dissolve_progress, "Dissolving progress");
	config.AddFloat2(tiling, "Tiling");

	//liquid properties
	config.AddFloat2(liquid_tiling_speed, "Liquid Tiling Speed");

	//colors
	config.AddColor(float4(diffuse_color[0], diffuse_color[1], diffuse_color[2], diffuse_color[3]), "difusseColor");
	config.AddColor(float4(emissive_color[0], emissive_color[1], emissive_color[2], 1.0f), "emissiveColor");
	config.AddColor(float4(specular_color[0], specular_color[1], specular_color[2], 1.0f), "specularColor");
}

void Material::Load(const Config& config)
{
	SetMaterialTexture(MaterialTextureType::DIFFUSE, config.GetUInt32("Diffuse", 0));
	SetMaterialTexture(MaterialTextureType::SPECULAR, config.GetUInt32("Specular", 0));
	SetMaterialTexture(MaterialTextureType::OCCLUSION, config.GetUInt32("Occlusion", 0));
	SetMaterialTexture(MaterialTextureType::EMISSIVE, config.GetUInt32("Emissive", 0));
	SetMaterialTexture(MaterialTextureType::NORMAL, config.GetUInt32("Normal", 0));
	SetMaterialTexture(MaterialTextureType::LIGHTMAP, config.GetUInt32("Lightmap", 0));
	SetMaterialTexture(MaterialTextureType::DISSOLVED_DIFFUSE, config.GetUInt32("Dissolved Diffuse", 0));
	SetMaterialTexture(MaterialTextureType::DISSOLVED_EMISSIVE, config.GetUInt32("Dissolved Emissive", 0));
	SetMaterialTexture(MaterialTextureType::NOISE, config.GetUInt32("Noise", 0));

	show_checkerboard_texture = config.GetBool("Checkboard", true);
	config.GetString("ShaderProgram", shader_program, "Blinn phong");

	material_type = static_cast<MaterialType>(config.GetInt("MaterialType", 0));

	transparency = config.GetFloat("Transparency", 1.f);
	reflection_strength = config.GetFloat("Reflection", 0.f);
	smoothness = config.GetFloat("Smoothness", 1.f);
	emissive_intensity = config.GetFloat("Emissive Intensity", 1.f);

	dissolve_progress = config.GetFloat("Dissolving progress", 0.f);
	config.GetFloat2("Tiling", tiling, float2::one);

	//liquid properties
	config.GetFloat2("Liquid Tiling Speed", liquid_tiling_speed, float2::one);

	//colors
	float4 diffuse;
	float4 emissive;
	float4 specular;

	config.GetColor("difusseColor", diffuse, float4(1.f, 1.f, 1.f, 1.f));
	config.GetColor("emissiveColor", emissive, float4(0.0f, 0.0f, 0.0f, 1.0f));
	config.GetColor("specularColor", specular, float4(0.025f, 0.025f, 0.025f, 0.025f));

	diffuse_color[0] = diffuse.x;
	diffuse_color[1] = diffuse.y;
	diffuse_color[2] = diffuse.z;
	diffuse_color[3] = diffuse.w;

	emissive_color[0] = emissive.x;
	emissive_color[1] = emissive.y;
	emissive_color[2] = emissive.z;
	emissive_color[3] = emissive.w;

	specular_color[0] = specular.x;
	specular_color[1] = specular.y;
	specular_color[2] = specular.z;
	specular_color[3] = specular.w;

}

void Material::LoadResource(uint32_t uuid, unsigned texture_type)
{
	MaterialTextureType type = static_cast<MaterialTextureType>(texture_type);
	textures[type] = std::static_pointer_cast<Texture>(App->resources->RetrieveFromCacheIfExist(uuid));


	if (textures[type])
	{
		return;
	}

	FileData file_data;
	bool succes = App->resources->RetrieveFileDataByUUID(uuid, file_data);
	if (succes)
	{
		//THINK WHAT TO DO IF IS IN CACHE
		textures[type] = ResourceManagement::Load<Texture>(uuid, file_data, true);

		//Delete file data buffer
		delete[] file_data.buffer;
		App->resources->AddResourceToCache(textures[type]);

	}

}

void Material::InitResource(uint32_t uuid, unsigned texture_type)
{
	MaterialTextureType type = static_cast<MaterialTextureType>(texture_type);
	if (textures[type] && !textures[type].get()->initialized)
	{
		textures[type].get()->LoadInMemory();
	}
}

void Material::RemoveMaterialTexture(MaterialTextureType type)
{
	textures[type] = nullptr;
}

void Material::SetMaterialTexture(MaterialTextureType type, uint32_t texture_uuid)
{
	textures_uuid[type] = texture_uuid;

	if (textures_uuid[type] != 0)
	{
		App->resources->loading_thread_communication.texture_type = type;
		App->resources->loading_thread_communication.current_type = ResourceType::TEXTURE;
		textures[type] = App->resources->Load<Texture>(texture_uuid);
	}
}

const std::shared_ptr<Texture>& Material::GetMaterialTexture(MaterialTextureType type) const
{
	return textures[type];
}

bool Material::UseLightmap() const
{
	return  textures[MaterialTextureType::LIGHTMAP] != nullptr;
}

void Material::ChangeTypeOfMaterial(const MaterialType new_material_type)
{
	material_type = new_material_type;
}

std::string Material::GetMaterialTypeName(const MaterialType material_type)
{
	switch (material_type)
	{
		case MaterialType::MATERIAL_OPAQUE:
			return "Opaque";

		case MaterialType::MATERIAL_TRANSPARENT:
			return "Transparent";

		case MaterialType::MATERIAL_LIQUID:
			return "Liquid";

		case MaterialType::MATERIAL_DISSOLVING:
			return "Dissolving";

		default:
			return "";
	}
}

void Material::UpdateLiquidProperties()
{
	liquid_horizontal_normals_tiling += float2(liquid_tiling_speed.x * App->time->delta_time * 0.001f);
	liquid_vertical_normals_tiling += float2(liquid_tiling_speed.y * App->time->delta_time * 0.001f);
}

unsigned int Material::GetShaderVariation() const
{
	unsigned int variation = 0;
	if (textures[MaterialTextureType::SPECULAR] != nullptr)
	{
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_SPECULAR_MAP);
	}
	if (textures[MaterialTextureType::NORMAL] != nullptr)
	{
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_NORMAL_MAP);
	}
	if (textures[MaterialTextureType::LIGHTMAP] != nullptr)
	{
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_LIGHT_MAP);
	}
	if (material_type == MaterialType::MATERIAL_LIQUID)
	{
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_LIQUID_PROPERTIES);
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_DISSOLVING_PROPERTIES);
	}
	if (material_type == MaterialType::MATERIAL_DISSOLVING)
	{
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_DISSOLVING_PROPERTIES);
	}

	return variation;
}

void Material::SetDissolveProgress(float progress)
{
	dissolve_progress = progress;
}

void Material::SetFinalAddedColor(const float4& final_added_color)
{
	this->final_added_color = final_added_color;
}

std::shared_ptr<Material> Material::GetInstance()
{
	Config material_config;
	Save(material_config);
	App->resources->loading_thread_communication.load_scene_asyncronously = false;
	std::shared_ptr<Material> new_instance = std::make_shared<Material>(uuid);
	new_instance->Load(material_config);
	new_instance->dissolve_progress = dissolve_progress;
	App->resources->loading_thread_communication.load_scene_asyncronously = MULTITHREADING;
	return new_instance;
}
