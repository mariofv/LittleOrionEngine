#include "ComponentMeshRenderer.h"

#include "ComponentAnimation.h"
#include "Component/ComponentMeshCollider.h"
#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleLight.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTexture.h"

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
	config.AddInt(properties, "Properties");
}

void ComponentMeshRenderer::SpecializedLoad(const Config& config)
{
	mesh_uuid = config.GetUInt32("Mesh", 0);
	SetMesh(mesh_uuid);

	material_uuid = config.GetUInt32("Material", 0);
	SetMaterial(material_uuid);

	skeleton_uuid =	config.GetUInt32("Skeleton", 0);
	SetSkeleton(skeleton_uuid);

	properties = config.GetInt("Properties", MeshProperties::RAYCASTABLE);
}

void ComponentMeshRenderer::LoadResource(uint32_t uuid, ResourceType resource, unsigned texture_type)
{
	APP_LOG_INFO("GO Loading Resource: %s", std::to_string(owner->UUID).c_str());

	if(resource == ResourceType::TEXTURE)
	{
		material_to_render->LoadResource(uuid, texture_type);
		APP_LOG_INFO("INITIALAZING TEXTURE: %s on component %s", std::to_string(uuid).c_str(), std::to_string(this->UUID).c_str());
	}
	else if(resource == ResourceType::MESH)
	{
		mesh_to_render = std::static_pointer_cast<Mesh>(App->resources->RetrieveFromCacheIfExist(uuid));
		APP_LOG_INFO("INITIALAZING MESH: %s on component %s", std::to_string(uuid).c_str(), std::to_string(this->UUID).c_str());
		if (mesh_to_render)
		{
			if(mesh_collider)
			{
				mesh_collider->InitMeshCollider();
			}

			return;
		}

		FileData file_data;
		bool succes = App->resources->RetrieveFileDataByUUID(uuid, file_data);
		if (succes)
		{
			//THINK WHAT TO DO IF IS IN CACHE
			mesh_to_render = ResourceManagement::Load<Mesh>(uuid, file_data, true);
			//Delete file data buffer
			delete[] file_data.buffer;
			App->resources->AddResourceToCache(std::static_pointer_cast<Resource>(mesh_to_render));
		
			if (mesh_collider)
			{
				mesh_collider->InitMeshCollider();
			}
		}
	}
}

void ComponentMeshRenderer::InitResource(uint32_t uuid, ResourceType resource, unsigned texture_type)
{
	if (resource == ResourceType::TEXTURE)
	{
		material_to_render->InitResource(uuid, texture_type);
	}
	else if (resource == ResourceType::MESH)
	{
		if (mesh_to_render && !mesh_to_render.get()->initialized)
		{
			mesh_to_render.get()->LoadInMemory();
			owner->aabb.GenerateBoundingBox();
		}
	}
}

void ComponentMeshRenderer::ReassignResource()
{
	SetMesh(mesh_uuid);
	SetMaterial(material_uuid);
}

GLuint ComponentMeshRenderer::BindShaderProgram() const
{
	unsigned int shader_variation = material_to_render->GetShaderVariation();
	if (App->renderer->shadows_enabled && IsPropertySet(MeshProperties::SHADOW_RECEIVER))
	{
		shader_variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_RECEIVE_SHADOWS);
	}
	if (App->renderer->cascade_mapping)
	{
		shader_variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_CASCADE_MAPPING);

		if (App->renderer->cascade_debug)
		{
			shader_variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_CASCADE_VISUALIZATION);
		}
	}
	if (App->renderer->bloom)
	{
		shader_variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_BLOOM);
	}


	return App->program->UseProgram(material_to_render->shader_program, shader_variation);
}

GLuint ComponentMeshRenderer::BindDepthShaderProgram() const
{
	return App->program->UseProgram("Depth", 0);
}

void ComponentMeshRenderer::BindMeshUniforms(GLuint shader_program) const
{
	glUniform1i(glGetUniformLocation(shader_program, "num_joints"), skeleton_uuid != 0 ? MAX_JOINTS : 1);

	if (palette.size() > 0)
	{
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "palette"), palette.size(), GL_TRUE, palette[0].ptr());
	}
	glUniform1i(glGetUniformLocation(shader_program, "has_skinning_value"), skeleton_uuid != 0 ? 0 : 1);

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), owner->transform.GetGlobalModelMatrix().Transposed().ptr());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void ComponentMeshRenderer::BindMaterialUniforms(GLuint shader_program) const
{
	BROFILER_CATEGORY("Render material", Profiler::Color::ForestGreen);
	AddDiffuseUniforms(shader_program);
	AddEmissiveUniforms(shader_program);
	AddSpecularUniforms(shader_program);
	AddFinalAddedColorUniform(shader_program);

	AddAmbientOclusionUniforms(shader_program);
	AddNormalUniforms(shader_program);
	AddLightMapUniforms(shader_program);

	if (material_to_render->material_type == Material::MaterialType::MATERIAL_DISSOLVING || material_to_render->material_type == Material::MaterialType::MATERIAL_LIQUID)
	{
		AddDissolveMaterialUniforms(shader_program);
	}

	if (material_to_render->material_type == Material::MaterialType::MATERIAL_LIQUID)
	{
		material_to_render->UpdateLiquidProperties();
		AddLiquidMaterialUniforms(shader_program);
	}

	AddExtraUniforms(shader_program);
}

void ComponentMeshRenderer::RenderModel() const
{
	BROFILER_CATEGORY("Draw call", Profiler::Color::LawnGreen);
	if (mesh_to_render == nullptr || !mesh_to_render->initialized)
	{
		return;
	}	
	glBindVertexArray(mesh_to_render->GetVAO());
	glDrawElements(GL_TRIANGLES, mesh_to_render->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ComponentMeshRenderer::AddDiffuseUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE3);
	BindTexture(Material::MaterialTextureType::DIFFUSE);
	glUniform1i(glGetUniformLocation(shader_program, "material.diffuse_map"), 3);
	
	glUniform4fv(glGetUniformLocation(shader_program, "material.diffuse_color"), 1, (float*)material_to_render->diffuse_color);

}

void ComponentMeshRenderer::AddEmissiveUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE4);
	BindTexture(Material::MaterialTextureType::EMISSIVE);
	glUniform1i(glGetUniformLocation(shader_program, "material.emissive_map"), 4);
	glUniform4fv(glGetUniformLocation(shader_program, "material.emissive_color"), 1, (float*)material_to_render->emissive_color);
	glUniform1f(glGetUniformLocation(shader_program, "material.emissive_intensity"), material_to_render->emissive_intensity);
}

void ComponentMeshRenderer::AddSpecularUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE5);
	BindTexture(Material::MaterialTextureType::SPECULAR);

	glUniform1i(glGetUniformLocation(shader_program, "material.specular_map"), 5);
	glUniform4fv(glGetUniformLocation(shader_program, "material.specular_color"), 1, (float*)material_to_render->specular_color);
	glUniform1f(glGetUniformLocation(shader_program, "material.smoothness"), material_to_render->smoothness);
}

void ComponentMeshRenderer::AddFinalAddedColorUniform(unsigned int shader_program) const
{
	glUniform4fv(glGetUniformLocation(shader_program, "material.final_added_color"), 1, material_to_render->final_added_color.ptr());
}

void ComponentMeshRenderer::AddAmbientOclusionUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE6);
	BindTexture(Material::MaterialTextureType::OCCLUSION);
	glUniform1i(glGetUniformLocation(shader_program, "material.occlusion_map"), 6);
}

void ComponentMeshRenderer::AddNormalUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE7);
	BindTexture(Material::MaterialTextureType::NORMAL);
	glUniform1i(glGetUniformLocation(shader_program, "material.normal_map"), 7);
}

void ComponentMeshRenderer::AddLightMapUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE8);
	BindTexture(Material::MaterialTextureType::LIGHTMAP);
	glUniform1i(glGetUniformLocation(shader_program, "material.light_map"), 8);
}

void ComponentMeshRenderer::AddLiquidMaterialUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE12);
	BindTexture(Material::MaterialTextureType::LIQUID);
	glUniform1i(glGetUniformLocation(shader_program, "material.liquid_map"), 12);
	glUniform2fv(glGetUniformLocation(shader_program, "material.liquid_horizontal_normals_tiling"), 1, material_to_render->liquid_horizontal_normals_tiling.ptr());
	glUniform2fv(glGetUniformLocation(shader_program, "material.liquid_vertical_normals_tiling"), 1, material_to_render->liquid_vertical_normals_tiling.ptr());
}

void ComponentMeshRenderer::AddDissolveMaterialUniforms(unsigned int shader_program) const
{
	glActiveTexture(GL_TEXTURE9);
	BindTexture(Material::MaterialTextureType::DISSOLVED_DIFFUSE);
	glUniform1i(glGetUniformLocation(shader_program, "material.dissolved_diffuse"), 9);
	glActiveTexture(GL_TEXTURE10);
	BindTexture(Material::MaterialTextureType::DISSOLVED_EMISSIVE);
	glUniform1i(glGetUniformLocation(shader_program, "material.dissolved_emissive"), 10);

	glActiveTexture(GL_TEXTURE11);
	BindTexture(Material::MaterialTextureType::NOISE);
	glUniform1i(glGetUniformLocation(shader_program, "material.dissolved_noise"), 11);

	glUniform1f(glGetUniformLocation(shader_program, "material.dissolve_progress"), material_to_render->dissolve_progress);
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
	glUniform1f(glGetUniformLocation(shader_program, "material.reflection_strength"), material_to_render->reflection_strength);
	glUniform2fv(glGetUniformLocation(shader_program, "material.tiling"), 1, material_to_render->tiling.ptr());

	//Ambient light intesity and color
	glUniform1f(glGetUniformLocation(shader_program, "ambient_light_intensity"), App->lights->ambient_light_intensity);
	glUniform4fv(glGetUniformLocation(shader_program, "ambient_light_color"), 1, (float*)App->lights->ambient_light_color);
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
		if (id == Material::MaterialTextureType::EMISSIVE || id == Material::MaterialTextureType::DISSOLVED_EMISSIVE)
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

Component* ComponentMeshRenderer::Clone(GameObject* owner, bool original_prefab)
{
	ComponentMeshRenderer* created_component;
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

	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);

	if(created_component)
	{
		created_component->ReassignResource();
	}

	return created_component;
}

void ComponentMeshRenderer::CopyTo(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentMeshRenderer*>(component_to_copy) = *this;
};

void ComponentMeshRenderer::SetMesh(uint32_t mesh_uuid)
{
	//Prepare multithreading loading
	App->resources->loading_thread_communication.current_component_loading = this;
	this->mesh_uuid = mesh_uuid;
	if (mesh_uuid != 0)
	{
		App->resources->loading_thread_communication.current_type = ResourceType::MESH;
		this->mesh_to_render = App->resources->Load<Mesh>(mesh_uuid);
		owner->aabb.GenerateBoundingBox();
	}
	
	App->resources->loading_thread_communication.current_component_loading = nullptr;
}

void ComponentMeshRenderer::SetMaterial(uint32_t material_uuid)
{
	//Prepare multithreading loading
	App->resources->loading_thread_communication.current_component_loading = this;

	this->material_uuid = material_uuid;
	if (material_uuid != 0)
	{
		material_to_render = App->resources->Load<Material>(material_uuid);
	}
	else
	{
		material_to_render = App->resources->Load<Material>((uint32_t)CoreResource::DEFAULT_MATERIAL);
	}

	//Set to default loading component
	App->resources->loading_thread_communication.current_component_loading = nullptr;
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

void ComponentMeshRenderer::UpdatePalette(std::vector<float4x4>& pose)
{
	assert(pose.size() == palette.size());
	const auto &  joints = skeleton->skeleton;
	for (size_t i = 0; i < pose.size(); ++i)
	{
		size_t parent_index = joints[i].parent_index;
		if (parent_index <= pose.size())
		{
			pose[i] = pose[parent_index] * pose[i];
		}
		palette[i] = pose[i] * joints[i].transform_global;
	}
}

bool ComponentMeshRenderer::IsPropertySet(MeshProperties property_to_check) const
{
	return properties & (int)property_to_check;
}

void ComponentMeshRenderer::AddProperty(MeshProperties property_to_add)
{
	properties |= (int)property_to_add;
}

void ComponentMeshRenderer::RemoveProperty(MeshProperties property_to_remove)
{
	properties &= ~(int)property_to_remove;
}

ENGINE_API void ComponentMeshRenderer::SetShadowCaster(bool caster)
{
	if(caster)
	{
		AddProperty(MeshProperties::SHADOW_CASTER);
	}
	else
	{
		RemoveProperty(MeshProperties::SHADOW_CASTER);
	}
}

bool ComponentMeshRenderer::CheckFilters(int filters) const
{
	return (filters & properties) == filters;
}
