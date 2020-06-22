#include "ComponentMeshRenderer.h"

#include "ComponentAnimation.h"
#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleCamera.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleTime.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

ComponentMeshRenderer::ComponentMeshRenderer(GameObject * owner) : Component(owner, ComponentType::MESH_RENDERER)
{
	SetMesh(0);
	SetMaterial(0);
	owner->aabb.GenerateBoundingBox();
}

ComponentMeshRenderer::ComponentMeshRenderer() : Component(nullptr, ComponentType::MESH_RENDERER)
{
	SetMesh(0);
	SetMaterial(0);
}


void ComponentMeshRenderer::Delete()
{
	App->renderer->RemoveComponentMesh(this);
}

void ComponentMeshRenderer::SpecializedSave(Config& config) const
{
	config.AddUInt(mesh_uuid, "Mesh");
	config.AddUInt(material_uuid, "Material");
	config.AddUInt(skeleton_uuid, "Skeleton");
}

void ComponentMeshRenderer::SpecializedLoad(const Config& config)
{
	mesh_uuid = config.GetUInt("Mesh", 0);
	SetMesh(mesh_uuid);

	material_uuid = config.GetUInt("Material", 0);
	SetMaterial(material_uuid);

	skeleton_uuid =	config.GetUInt("Skeleton", 0);
	SetSkeleton(skeleton_uuid);
}

void ComponentMeshRenderer::Render()
{
	if (material_to_render == nullptr)
	{
		return;
	}
	std::string program_name = material_to_render->shader_program;
	GLuint program = App->program->GetShaderProgramId(program_name);
	glUseProgram(program);

	glUniform1i(glGetUniformLocation(program, "num_joints"), skeleton_uuid != 0 ? MAX_JOINTS : 1);
	
	if (palette.size() > 0)
	{
		glUniformMatrix4fv(glGetUniformLocation(program, "palette"), palette.size(), GL_TRUE, &palette[0][0][0]);
	}
	glUniform1i(glGetUniformLocation(program, "has_skinning_value"), skeleton_uuid != 0 ? 0 : 1);
	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), owner->transform.GetGlobalModelMatrix().Transposed().ptr());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	App->lights->Render(owner->transform.GetGlobalTranslation(), program);

	material_to_render->use_specular_map = material_to_render->GetMaterialTexture(Material::MaterialTextureType::SPECULAR) != nullptr;
	material_to_render->use_normal_map = material_to_render->GetMaterialTexture(Material::MaterialTextureType::NORMAL) != nullptr;

	RenderMaterial(program);
	RenderModel();

	glUseProgram(0);
}	

void ComponentMeshRenderer::RenderModel() const
{
	if (mesh_to_render == nullptr)
	{
		return;
	}	
	glBindVertexArray(mesh_to_render->GetVAO());
	glDrawElements(GL_TRIANGLES, mesh_to_render->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ComponentMeshRenderer::RenderMaterial(GLuint shader_program) const
{
	glUniform1i(glGetUniformLocation(shader_program, "time"), App->time->real_time_delta_time);
	AddDiffuseUniforms(shader_program);
	AddEmissiveUniforms(shader_program);
	AddSpecularUniforms(shader_program);

	AddAmbientOclusionUniforms(shader_program);
	AddNormalUniforms(shader_program);
	AddLightMapUniforms(shader_program);
	AddLiquidMaterialUniforms(shader_program);
	if (material_to_render->material_type == Material::MaterialType::MATERIAL_LIQUID)
	{
		material_to_render->UpdateLiquidProperties();
	}
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
	glUniform1i(glGetUniformLocation(shader_program, "material.use_specular_map"), material_to_render->use_specular_map);

	glUniform1i(glGetUniformLocation(shader_program, "material.specular_map"), 2);
	glUniform4fv(glGetUniformLocation(shader_program, "material.specular_color"), 1, (float*)material_to_render->specular_color);
	glUniform1f(glGetUniformLocation(shader_program, "material.k_specular"), material_to_render->k_specular);
	
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
	glActiveTexture(GL_TEXTURE4);
	BindTexture(Material::MaterialTextureType::NORMAL);
	glUniform1i(glGetUniformLocation(shader_program, "material.use_normal_map"), material_to_render->use_normal_map);
	glUniform1i(glGetUniformLocation(shader_program, "material.normal_map"), 4);
}

void ComponentMeshRenderer::AddLightMapUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE5);
	bool has_lightmap =  BindTexture(Material::MaterialTextureType::LIGHTMAP);
	glUniform1i(glGetUniformLocation(shader_program, "material.light_map"), 5);
	glUniform1i(glGetUniformLocation(shader_program, "use_light_map"), has_lightmap ? 1 : 0);
}
void ComponentMeshRenderer::AddLiquidMaterialUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE6);
	BindTexture(Material::MaterialTextureType::LIQUID);
	glUniform1i(glGetUniformLocation(shader_program, "material.liquid_map"), 6);
	glUniform1f(glGetUniformLocation(shader_program, "material.tiling_liquid_x_x"), material_to_render->tiling_liquid_x_x);
	glUniform1f(glGetUniformLocation(shader_program, "material.tiling_liquid_x_y"), material_to_render->tiling_liquid_x_y);
	glUniform1f(glGetUniformLocation(shader_program, "material.tiling_liquid_y_x"), material_to_render->tiling_liquid_y_x);
	glUniform1f(glGetUniformLocation(shader_program, "material.tiling_liquid_y_y"), material_to_render->tiling_liquid_y_y);
	glUniform1i(glGetUniformLocation(shader_program, "material.use_liquid_map"), material_to_render->use_liquid_map);
}
void ComponentMeshRenderer::AddExtraUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, App->cameras->directional_light_camera->depth_map);
	glUniform1i(glGetUniformLocation(shader_program, "close_depth_map"), 6);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, App->cameras->directional_light_mid->depth_map);
	glUniform1i(glGetUniformLocation(shader_program, "mid_depth_map"), 7);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, App->cameras->directional_light_far->depth_map);
	glUniform1i(glGetUniformLocation(shader_program, "far_depth_map"), 8);

	if (material_to_render->material_type == Material::MaterialType::MATERIAL_OPAQUE)
	{
		glUniform1f(glGetUniformLocation(shader_program, "material.transparency"), 1.f);
	}
	else
	{
		glUniform1f(glGetUniformLocation(shader_program, "material.transparency"), material_to_render->transparency);
	}

	glUniform1f(glGetUniformLocation(shader_program, "material.tiling_x"), material_to_render->tiling_x);
	glUniform1f(glGetUniformLocation(shader_program, "material.tiling_y"), material_to_render->tiling_y);

	//Ambient light intesity and color
	glUniform1f(glGetUniformLocation(shader_program, "ambient_light_intensity"), App->lights->ambient_light_intensity);
	glUniform4fv(glGetUniformLocation(shader_program, "ambient_light_color"), 1, (float*)App->lights->ambient_light_color);
	glUniform1i(glGetUniformLocation(shader_program, "render_shadows"), App->renderer->render_shadows);
}

bool ComponentMeshRenderer::BindTexture(Material::MaterialTextureType id) const
{
	bool valid_texture = material_to_render->textures[id] != nullptr;
	GLuint texture_id;
	if (material_to_render->show_checkerboard_texture)
	{
		texture_id = App->texture->checkerboard_texture_id;
	}
	else if (valid_texture)
	{
		texture_id = material_to_render->textures[id]->opengl_texture;
	}
	else
	{
		if (id == Material::MaterialTextureType::EMISSIVE)
		{
			texture_id = App->texture->blackfall_texture_id;
		}
		else
		{
			texture_id = App->texture->whitefall_texture_id;
		}
	}
	glBindTexture(GL_TEXTURE_2D, texture_id);
	return valid_texture;
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
	CloneBase(static_cast<Component*>(created_component));
	return created_component;
}

void ComponentMeshRenderer::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentMeshRenderer*>(component_to_copy) = *this;
};

void ComponentMeshRenderer::SetMesh(uint32_t mesh_uuid)
{
	this->mesh_uuid = mesh_uuid;
	if (mesh_uuid != 0)
	{
		this->mesh_to_render = App->resources->Load<Mesh>(mesh_uuid);
		owner->aabb.GenerateBoundingBox();
	}
}

void ComponentMeshRenderer::SetMaterial(uint32_t material_uuid)
{
	this->material_uuid = material_uuid;
	if (material_uuid != 0)
	{
		material_to_render = App->resources->Load<Material>(material_uuid);
	}
	else
	{
		material_to_render = App->resources->Load<Material>((uint32_t)CoreResource::DEFAULT_MATERIAL);
	}
}

void ComponentMeshRenderer::SetSkeleton(uint32_t skeleton_uuid)
{
	this->skeleton_uuid = skeleton_uuid;
	if (skeleton_uuid != 0)
	{
		skeleton = App->resources->Load<Skeleton>(skeleton_uuid);
		palette.resize(skeleton ? skeleton->skeleton.size() : 0 );
		for (auto & matrix : palette)
		{
			matrix = float4x4::identity;
		}
	}
}

void ComponentMeshRenderer::UpdatePalette(const std::vector<float4x4>& pose)
{
	assert(pose.size() == palette.size());
	for (size_t i = 0; i < pose.size(); ++i)
	{
		auto &  joints = skeleton->skeleton;
		size_t joint_index = i;
		float4x4 global_transform = float4x4::identity;
		while (joints[joint_index].parent_index != -1)
		{
			joint_index = joints[joint_index].parent_index;
			global_transform = pose[joint_index] * global_transform;

		}
		palette[i] =  global_transform * pose[i] * joints[i].transform_global;
	}
}
