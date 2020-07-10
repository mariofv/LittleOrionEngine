#include "Material.h"

#include "Helper/Config.h"
#include "Main/Application.h"

#include "Module/ModuleLight.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleTime.h"


#include "ResourceManagement/Metafile/Metafile.h"

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
		default:
			break;
			
		}
	}

	config.AddInt((int)material_type, "MaterialType");

	config.AddBool(show_checkerboard_texture, "Checkboard");
	config.AddString(shader_program, "ShaderProgram");

	config.AddFloat(smoothness, "Smoothness");

	config.AddFloat(transparency, "Transparency");

	config.AddFloat(tiling_x, "Tiling X");
	config.AddFloat(tiling_y, "Tiling Y");

	//liquid properties

	config.AddFloat(speed_tiling_x, "Speed Liquid Map X");
	config.AddFloat(speed_tiling_y, "Speed Liquid Map Y");

	config.AddFloat(tiling_liquid_x_x, "Tiling Liquid Map 1 x");
	config.AddFloat(tiling_liquid_x_y, "Tiling Liquid Map 1 y");
	config.AddFloat(tiling_liquid_y_x, "Tiling Liquid Map 2 x");
	config.AddFloat(tiling_liquid_y_y, "Tiling Liquid Map 2 y");
	config.AddBool(use_liquid_map, "Use Liquid Map");

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
	
	show_checkerboard_texture = config.GetBool("Checkboard", true);
	config.GetString("ShaderProgram", shader_program, "Blinn phong");

	material_type = static_cast<MaterialType>(config.GetInt("MaterialType", 0));

	transparency = config.GetFloat("Transparency", 1.f);
	smoothness = config.GetFloat("Smoothness", 1.0F);


	tiling_x = config.GetFloat("Tiling X", 1.0f);
	tiling_y = config.GetFloat("Tiling Y", 1.0f);

	//liquid properties

	speed_tiling_x = config.GetFloat("Speed Liquid Map X", 1.0F);
	speed_tiling_y = config.GetFloat("Speed Liquid Map Y", 1.0F);

	tiling_liquid_x_x = config.GetFloat("Tiling Liquid Map 1 x", 1.0F);
	tiling_liquid_x_y = config.GetFloat("Tiling Liquid Map 1 y", 1.0F);
	tiling_liquid_y_x = config.GetFloat("Tiling Liquid Map 2 x", 1.0F);
	tiling_liquid_y_y = config.GetFloat("Tiling Liquid Map 2 y", 1.0F);
	use_liquid_map = config.GetBool("Use Liquid Map", false);



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

void Material::RemoveMaterialTexture(MaterialTextureType type)
{
	textures[type] = nullptr;
	
}

void Material::SetMaterialTexture(MaterialTextureType type, uint32_t texture_uuid)
{
	textures_uuid[type] = texture_uuid;
	if (textures_uuid[type] != 0)
	{
		textures[type] = App->resources->Load<Texture>(texture_uuid);
	}
	use_liquid_map = type == MaterialTextureType::LIQUID && texture_uuid != 0;
}

const std::shared_ptr<Texture>& Material::GetMaterialTexture(MaterialTextureType type) const
{
	return textures[type];
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

		default:
			return "";
	}
}

void Material::UpdateLiquidProperties()
{
	//TODO->change it to liquid maps and not hardcoded
	//tiling_liquid_x_x += speed_tiling_x / 1000 * App->time->delta_time;
	//tiling_liquid_x_y += speed_tiling_x / 1000 * App->time->delta_time;
	tiling_liquid_y_x -= speed_tiling_y / 1000 * App->time->delta_time;
	tiling_liquid_y_y -= speed_tiling_y / 1000 * App->time->delta_time;
	tiling_liquid_x_x += speed_tiling_x / 1000 * App->time->delta_time;
	tiling_liquid_x_y += speed_tiling_y / 1000 * App->time->delta_time;
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
	if (use_shadow_map && App->lights->render_shadows) 
	{
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_RECEIVE_SHADOWS);
	}

	return variation;
}