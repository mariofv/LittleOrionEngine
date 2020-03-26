#include "Material.h"

#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleResourceManager.h"

Material::Material(uint32_t UUID, std::string material_file_path) :
	Resource(UUID, material_file_path)
{
	textures.resize(MAX_MATERIAL_TEXTURE_TYPES);
}

void Material::Save(Config& config) const
{
	for (size_t i = 0; i < textures.size(); i++)
	{
		if (textures[i] != nullptr)
		{
			MaterialTextureType type = static_cast<MaterialTextureType>(i);

			switch (type)
			{
			case MaterialTextureType::DIFFUSE:
				config.AddString(textures[i]->exported_file, "Diffuse");
				break;

			case MaterialTextureType::SPECULAR:
				config.AddString(textures[i]->exported_file, "Specular");
				break;

			case MaterialTextureType::OCCLUSION:
				config.AddString(textures[i]->exported_file, "Occlusion");
				break;

			case MaterialTextureType::EMISSIVE:
				config.AddString(textures[i]->exported_file, "Emissive");
				break;

			default:
				break;
			}
		}
	}

	config.AddBool(show_checkerboard_texture, "Checkboard");
	config.AddString(shader_program, "ShaderProgram");

	//k
	config.AddFloat(k_ambient, "kAmbient");
	config.AddFloat(k_specular, "kSpecular");
	config.AddFloat(k_diffuse, "kDiffuse");
	config.AddFloat(shininess, "shininess");

	//colors
	config.AddColor(float4(diffuse_color[0], diffuse_color[1], diffuse_color[2], diffuse_color[3]), "difusseColor");
	config.AddColor(float4(emissive_color[0], emissive_color[1], emissive_color[2], 1.0f), "emissiveColor");
	config.AddColor(float4(specular_color[0], specular_color[1], specular_color[2], 1.0f), "specularColor");
}

void Material::Load(const Config& config)
{
	std::string texture_path;
	config.GetString("Diffuse", texture_path, "");
	std::shared_ptr<Texture> texture_resource = App->resources->Load<Texture>(texture_path);
	if (texture_resource.get() != nullptr)
	{
		SetMaterialTexture(Material::MaterialTextureType::DIFFUSE, texture_resource);
	}

	config.GetString("Specular", texture_path, "");
	texture_resource = App->resources->Load<Texture>(texture_path);
	if (texture_resource.get() != nullptr)
	{
		SetMaterialTexture(Material::MaterialTextureType::SPECULAR, texture_resource);
	}

	config.GetString("Occlusion", texture_path, "");
	texture_resource = App->resources->Load<Texture>(texture_path);
	if (texture_resource.get() != nullptr)
	{
		SetMaterialTexture(Material::MaterialTextureType::OCCLUSION, texture_resource);
	}

	config.GetString("Emissive", texture_path, "");
	texture_resource = App->resources->Load<Texture>(texture_path);
	if (texture_resource.get() != nullptr)
	{
		SetMaterialTexture(Material::MaterialTextureType::EMISSIVE, texture_resource);
	}

	show_checkerboard_texture = config.GetBool("Checkboard", true);
	config.GetString("ShaderProgram", shader_program, "Blinn phong");

	//k
	k_ambient = config.GetFloat("kAmbient", 1.0f);
	k_specular = config.GetFloat("kSpecular", 1.0f);
	k_diffuse = config.GetFloat("kDiffuse", 1.0f);
	shininess = config.GetFloat("shininess", 1.0f);

	//colors
	float4 diffuse;
	float4 emissive;
	float4 specular;

	config.GetColor("difusseColor", diffuse, float4(1.f, 1.f, 1.f, 1.f));
	config.GetColor("emissiveColor", emissive, float4(0.0f, 0.0f, 0.0f, 1.0f));
	config.GetColor("specularColor", specular, float4(0.0f, 0.0f, 0.0f, 1.0f));

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

void Material::SetMaterialTexture(MaterialTextureType type, const std::shared_ptr<Texture> & new_texture)
{
	textures[type] = new_texture;
}

const std::shared_ptr<Texture>& Material::GetMaterialTexture(MaterialTextureType type) const
{
	return textures[type];
}
