#include "ComponentMaterial.h"

#include "Main/Application.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleResourceManager.h"

ComponentMaterial::ComponentMaterial() : Component(nullptr, ComponentType::MATERIAL)
{
	textures.resize(Texture::MAX_TEXTURE_TYPES);
}

ComponentMaterial::ComponentMaterial(GameObject * owner) : Component(owner, ComponentType::MATERIAL)
{
	textures.resize(Texture::MAX_TEXTURE_TYPES);
}

ComponentMaterial::~ComponentMaterial()
{
	for (auto & texture : textures)
	{
		App->resources->RemoveResourceFromCacheIfNeeded(texture);
	}
}

void ComponentMaterial::Delete()
{
	App->texture->RemoveComponentMaterial(this);
}

void ComponentMaterial::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddInt((unsigned int)type, "ComponentType");
	config.AddBool(active, "Active");
	config.AddInt(index, "Index");
	for (size_t i = 0; i< textures.size(); i++ )
	{
		if (textures[i] != nullptr)
		{
			std::string id = "Path" + i;
			config.AddString(textures[i]->exported_file, id);
		}
	}
	config.AddBool(show_checkerboard_texture, "Checkboard");
	config.AddString(shader_program, "ShaderProgram");

	//k
	config.AddFloat(k_ambient, "kAmbient");
	config.AddFloat(k_specular, "kSpecular");
	config.AddFloat(k_diffuse, "kDiffuse");
	config.AddFloat(shininess, "shininess");
	config.AddFloat(alpha_blending, "alpha_blending");

	//colors
	config.AddColor(float4(diffuse_color[0], diffuse_color[1], diffuse_color[2], diffuse_color[3]), "difusseColor");
	config.AddColor(float4(emissive_color[0], emissive_color[1], emissive_color[2], 1.0f), "emissiveColor");
	config.AddColor(float4(specular_color[0], specular_color[1], specular_color[2], 1.0f), "specularColor");
}

void ComponentMaterial::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);

	index = config.GetInt("Index", 0);

	std::string tmp_path;
	config.GetString("Path", tmp_path, "");
	textures.resize(Texture::MAX_TEXTURE_TYPES);
	for (size_t i = 0; i < textures.size(); i++)
	{
		std::string id = "Path" + i;
		std::string tmp_path;
		config.GetString(id, tmp_path, "");
		if (!tmp_path.empty())
		{
			textures[i] = App->resources->Load<Texture>(tmp_path);
		}
	}

	show_checkerboard_texture = config.GetBool("Checkboard", true);
	config.GetString("ShaderProgram", shader_program, "Blinn phong");

	//k
	k_ambient = config.GetFloat("kAmbient", 1.0f);
	k_specular = config.GetFloat( "kSpecular", 1.0f);
	k_diffuse = config.GetFloat("kDiffuse", 1.0f);
	k_normal = config.GetFloat("kNormal", 1.0f);
	shininess = config.GetFloat("shininess", 1.0f);
	roughness = config.GetFloat("roughness", 0.5f);
	metalness = config.GetFloat("metalness", 0.04f);
	alpha_blending = config.GetFloat("alpha_blending", 1.0f);

	//colors
	float4 diffuse;
	float4 emissive;
	float4 specular;

	config.GetColor("difusseColor", diffuse, float4(1.f, 1.f, 1.f, 1.f));
	config.GetColor("emissiveColor", emissive, float4(0.0f,0.0f,0.0f,1.0f));
	config.GetColor("specularColor", specular, float4(0.0f,0.0f,0.0f,1.0f));

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

void ComponentMaterial::Render(unsigned int shader_program) const
{
	AddDiffuseUniforms(shader_program);
	AddEmissiveUniforms(shader_program);
	AddSpecularUniforms(shader_program);
	AddAmbientOclusionUniforms(shader_program);
	AddNormalUniforms(shader_program);
	AddExtraUniforms(shader_program);
}

void ComponentMaterial::AddDiffuseUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE0);
	BindTexture(Texture::TextureType::DIFUSSE);
	glUniform1i(glGetUniformLocation(shader_program, "material.diffuse_map"), 0);
	glUniform4fv(glGetUniformLocation(shader_program, "material.diffuse_color"), 1, (float*)diffuse_color);
	glUniform1f(glGetUniformLocation(shader_program, "material.k_diffuse"),  k_diffuse);

}

void ComponentMaterial::AddEmissiveUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE1);
	BindTexture(Texture::TextureType::EMISSIVE);
	glUniform1i(glGetUniformLocation(shader_program, "material.emissive_map"), 1);
	glUniform4fv(glGetUniformLocation(shader_program, "material.emissive_color"), 1, (float*)emissive_color);
}

void ComponentMaterial::AddSpecularUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE2);
	BindTexture(Texture::TextureType::SPECULAR);
	glUniform1i(glGetUniformLocation(shader_program, "material.specular_map"), 2);
	glUniform4fv(glGetUniformLocation(shader_program, "material.specular_color"), 1, (float*)specular_color);
	glUniform1f(glGetUniformLocation(shader_program, "material.k_specular"), k_specular);

	//Material BRDF variables
	glUniform1f(glGetUniformLocation(shader_program, "material.roughness"), roughness);
	glUniform1f(glGetUniformLocation(shader_program, "material.metalness"), metalness);

}

void ComponentMaterial::AddAmbientOclusionUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE3);
	BindTexture(Texture::TextureType::OCLUSION);
	glUniform1i(glGetUniformLocation(shader_program, "material.occlusion_map"), 3);
	glUniform1f(glGetUniformLocation(shader_program, "material.k_ambient"), k_ambient);
}

void ComponentMaterial::AddNormalUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE3);
	BindTexture(Texture::TextureType::NORMAL);
	glUniform1i(glGetUniformLocation(shader_program, "material.normal_map"), 4);
	bool texture_normal = BindTextureNormal(Texture::TextureType::NORMAL);
	//if (texture_normal)
	//{
	//	unsigned index = glGetSubroutineIndex(shader_program, GL_FRAGMENT_SHADER,"ComputeMaterialWithNormalMap");
	//	unsigned indices[1]; // NUMBER_SUBROUTINES == 1 in our example
	//	unsigned location = glGetSubroutineUniformLocation(shader_program, GL_FRAGMENT_SHADER,"NormalSoubroutine");
	//	indices[index] = location;
	//	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, indices);
	//}
	//else {
	//	GLint index = glGetSubroutineIndex(shader_program, GL_FRAGMENT_SHADER, "ComputeMaterialWithoutNormalMap"); 
	//	unsigned indices[1]; 
	//	unsigned location = glGetSubroutineUniformLocation(shader_program, GL_FRAGMENT_SHADER, "NormalSoubroutine");
	//	indices[index] = location;
	//	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, indices);
	//}
}
void ComponentMaterial::AddExtraUniforms(unsigned int shader_program) const
{
	glUniform1f(glGetUniformLocation(shader_program, "material.alpha_blending"), alpha_blending);
}

void ComponentMaterial::BindTexture(Texture::TextureType id) const
{
	GLuint texture_id;
	if (show_checkerboard_texture)
	{
		texture_id = App->texture->checkerboard_texture_id;
	}
	else if (textures[id] != nullptr)
	{
		texture_id = textures[id]->opengl_texture;
	}
	else
	{
		texture_id = App->texture->whitefall_texture_id;
	}
	glBindTexture(GL_TEXTURE_2D, texture_id);
}
bool ComponentMaterial::BindTextureNormal(Texture::TextureType id) const
{
	GLuint texture_id;
	
	if (textures[id] != nullptr)
	{
		texture_id = textures[id]->opengl_texture;
		glBindTexture(GL_TEXTURE_2D, texture_id);
		return true;
	}
	else {
	
		return false;
	}
	
	
}
void ComponentMaterial::RemoveMaterialTexture(size_t type)
{
	App->resources->RemoveResourceFromCacheIfNeeded(textures[type]);
	textures[type] = nullptr;
}

void ComponentMaterial::SetMaterialTexture(size_t type, const std::shared_ptr<Texture> & new_texture)
{
	textures[type] = new_texture;
}

const std::shared_ptr<Texture>& ComponentMaterial::GetMaterialTexture(size_t  type) const
{
	return textures[type];
}

const char* ComponentMaterial::TypeOfMaterial(const MaterialType material_type)
{
	switch (material_type)
	{
		case ComponentMaterial::MaterialType::MATERIALOPAQUE:
			return "Opaque";
			break;
		case ComponentMaterial::MaterialType::MATERIALTRANSPARENT:
			return "Transparent";
			break;
	}
}

void ComponentMaterial::ChangeTypeOfMaterial(const MaterialType new_material_type)
{
	material_type = new_material_type;
}