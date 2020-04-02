#include "ComponentMeshRenderer.h"

#include "ComponentAnimation.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleDebugDraw.h"

#include <algorithm>
ComponentMeshRenderer::ComponentMeshRenderer(const std::shared_ptr<Mesh> & mesh_to_render) : mesh_to_render(mesh_to_render), Component(nullptr, ComponentType::MESH_RENDERER)
{
	owner->aabb.GenerateBoundingBox();
}

ComponentMeshRenderer::ComponentMeshRenderer(const std::shared_ptr<Mesh> & mesh_to_render, GameObject * owner) : mesh_to_render(mesh_to_render), Component(owner, ComponentType::MESH_RENDERER)
{
	owner->aabb.GenerateBoundingBox();
}

ComponentMeshRenderer::ComponentMeshRenderer() : Component(nullptr, ComponentType::MESH_RENDERER)
{
	this->mesh_to_render = App->resources->Load<Mesh>(PRIMITIVE_CUBE_PATH);
	this->material_to_render = App->resources->Load<Material>(DEFAULT_MATERIAL_PATH);
}

void ComponentMeshRenderer::SetMesh(const std::shared_ptr<Mesh> & mesh_to_render)
{
	this->mesh_to_render = mesh_to_render;
	owner->aabb.GenerateBoundingBox();
}

void ComponentMeshRenderer::SetMaterial(const std::shared_ptr<Material> & material_to_render)
{
	this->material_to_render = material_to_render;
}


void ComponentMeshRenderer::Delete()
{
	App->renderer->RemoveComponentMesh(this);
}

void ComponentMeshRenderer::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddInt((unsigned int)type, "ComponentType");
	config.AddBool(active, "Active");
	config.AddString(mesh_to_render->exported_file, "MeshPath");
	config.AddString(material_to_render->exported_file, "MaterialPath");
}

void ComponentMeshRenderer::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);

	std::string mesh_path;
	config.GetString("MeshPath", mesh_path, "");
	std::shared_ptr<Mesh> mesh = App->resources->Load<Mesh>(mesh_path.c_str());
	if (mesh != nullptr)
	{
		SetMesh(mesh);
	}
	else 
	{
		SetMesh(App->resources->Load<Mesh>(PRIMITIVE_CUBE_PATH));
	}

	std::string material_path;
	config.GetString("MaterialPath", material_path, "");
	std::shared_ptr<Material> material = App->resources->Load<Material>(material_path);
	if (material != nullptr)
	{
		SetMaterial(material);
	}
	else
	{
		SetMaterial(App->resources->Load<Material>(DEFAULT_MATERIAL_PATH));
	}

}

void ComponentMeshRenderer::Render() const
{
	if (material_to_render->material_type == Material::MaterialType::MATERIAL_TRANSPARENT)
	{

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		/*glBlendFunc(GL_ONE, GL_ONE); TODO -> FIX THIS
		glBlendEquation(GL_FUNC_ADD);*/
	}
	std::string program_name = material_to_render->shader_program;
	GLuint program = App->program->GetShaderProgramId(program_name);
	glUseProgram(program);

	ComponentAnimation* anim = static_cast<ComponentAnimation*>(owner->parent->GetComponent(ComponentType::ANIMATION));
	if (anim != nullptr)
	{
		anim->Render(program);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), owner->transform.GetGlobalModelMatrix().Transposed().ptr());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	App->lights->Render(owner->transform.GetGlobalTranslation(), program);
	RenderMaterial(program);
	RenderModel();
	if (material_to_render->material_type == Material::MaterialType::MATERIAL_TRANSPARENT)
	{
		glDisable(GL_BLEND);
	}
	glUseProgram(0);
}	

void ComponentMeshRenderer::RenderModel() const
{
	glBindVertexArray(mesh_to_render->GetVAO());
	glDrawElements(GL_TRIANGLES, mesh_to_render->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ComponentMeshRenderer::RenderMaterial(GLuint shader_program) const
{
	
	AddDiffuseUniforms(shader_program);
	AddEmissiveUniforms(shader_program);
	AddSpecularUniforms(shader_program);
	AddAmbientOclusionUniforms(shader_program);
	AddNormalUniforms(shader_program);
	AddExtraUniforms(shader_program);
	
}


void ComponentMeshRenderer::AddDiffuseUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE0);
	BindTexture(Material::MaterialTextureType::DIFFUSE);
	glUniform1i(glGetUniformLocation(shader_program, "material.diffuse_map"), 0);
	glUniform4fv(glGetUniformLocation(shader_program, "material.diffuse_color"), 1, (float*)material_to_render->diffuse_color);
	glUniform1f(glGetUniformLocation(shader_program, "material.k_diffuse"), material_to_render->k_diffuse);

}

void ComponentMeshRenderer::AddEmissiveUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE1);
	BindTexture(Material::MaterialTextureType::EMISSIVE);
	glUniform1i(glGetUniformLocation(shader_program, "material.emissive_map"), 1);
	glUniform4fv(glGetUniformLocation(shader_program, "material.emissive_color"), 1, (float*)material_to_render->emissive_color);
}

void ComponentMeshRenderer::AddSpecularUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE2);
	BindTexture(Material::MaterialTextureType::SPECULAR);
	glUniform1i(glGetUniformLocation(shader_program, "material.specular_map"), 2);
	glUniform4fv(glGetUniformLocation(shader_program, "material.specular_color"), 1, (float*)material_to_render->specular_color);
	glUniform1f(glGetUniformLocation(shader_program, "material.k_specular"), material_to_render->k_specular);
	glUniform1f(glGetUniformLocation(shader_program, "material.shininess"), material_to_render->specular_color[3]);

	//Material BRDF variables
	glUniform1f(glGetUniformLocation(shader_program, "material.roughness"), material_to_render->roughness);
	glUniform1f(glGetUniformLocation(shader_program, "material.metalness"), material_to_render->metalness);
}

void ComponentMeshRenderer::AddAmbientOclusionUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE3);
	BindTexture(Material::MaterialTextureType::OCCLUSION);
	glUniform1i(glGetUniformLocation(shader_program, "material.occlusion_map"), 3);
	glUniform1f(glGetUniformLocation(shader_program, "material.k_ambient"), material_to_render->k_ambient);
}

void ComponentMeshRenderer::AddNormalUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE3);
	BindTexture(Material::MaterialTextureType::NORMAL);
	glUniform1i(glGetUniformLocation(shader_program, "material.normal_map"), 4);
	//bool texture_normal = BindTextureNormal(Material::MaterialTextureType::NORMAL);
	//if (texture_normal)
	//{
	//    unsigned index = glGetSubroutineIndex(shader_program, GL_FRAGMENT_SHADER,"ComputeMaterialWithNormalMap");
	//    unsigned indices[1]; // NUMBER_SUBROUTINES == 1 in our example
	//    unsigned location = glGetSubroutineUniformLocation(shader_program, GL_FRAGMENT_SHADER,"NormalSoubroutine");
	//    indices[index] = location;
	//    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, indices);
	//}
	//else {
	//    GLint index = glGetSubroutineIndex(shader_program, GL_FRAGMENT_SHADER, "ComputeMaterialWithoutNormalMap"); 
	//    unsigned indices[1]; 
	//    unsigned location = glGetSubroutineUniformLocation(shader_program, GL_FRAGMENT_SHADER, "NormalSoubroutine");
	//    indices[index] = location;
	//    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, indices);
	//}
}

void ComponentMeshRenderer::AddExtraUniforms(unsigned int shader_program) const
{
	if (material_to_render->material_type == Material::MaterialType::MATERIAL_OPAQUE)
	{
		glUniform1f(glGetUniformLocation(shader_program, "material.transparency"), 1.f);
	}
	else
	{
		glUniform1f(glGetUniformLocation(shader_program, "material.transparency"), material_to_render->transparency);
	}
}

void ComponentMeshRenderer::BindTexture(Material::MaterialTextureType id) const
{
	GLuint texture_id;
	if (material_to_render->show_checkerboard_texture)
	{
		texture_id = App->texture->checkerboard_texture_id;
	}
	else if (material_to_render->textures[id] != nullptr)
	{
		texture_id = material_to_render->textures[id]->opengl_texture;
	}
	else
	{
		texture_id = App->texture->whitefall_texture_id;
	}
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

bool ComponentMeshRenderer::BindTextureNormal(Material::MaterialTextureType id) const
{
	GLuint texture_id;

	if (material_to_render->textures[id] != nullptr)
	{
		texture_id = material_to_render->textures[id]->opengl_texture;
		glBindTexture(GL_TEXTURE_2D, texture_id);
		return true;
	}
	else {

		return false;
	}
}

Component* ComponentMeshRenderer::Clone(bool original_prefab) const
{
	ComponentMeshRenderer * created_component;
	if (original_prefab)
	{
		created_component = new ComponentMeshRenderer();
	}
	else
	{
		created_component = App->renderer->CreateComponentMeshRenderer();
	}
	*created_component = *this;
	return created_component;
}

void ComponentMeshRenderer::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentMeshRenderer*>(component_to_copy) = *this;
};