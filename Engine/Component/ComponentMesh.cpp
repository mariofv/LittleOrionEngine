#include "ComponentMesh.h"

#include "Component/ComponentMaterial.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleResourceManager.h"

ComponentMesh::ComponentMesh(const std::shared_ptr<Mesh> & mesh_to_render) : mesh_to_render(mesh_to_render), Component(nullptr, ComponentType::MESH)
{
	owner->aabb.GenerateBoundingBox();
}

ComponentMesh::ComponentMesh(const std::shared_ptr<Mesh> & mesh_to_render, GameObject * owner) : mesh_to_render(mesh_to_render), Component(owner, ComponentType::MESH)
{
	owner->aabb.GenerateBoundingBox();
}

ComponentMesh::ComponentMesh() : Component(nullptr, ComponentType::MESH)
{
}

void ComponentMesh::SetMesh(const std::shared_ptr<Mesh> & mesh_to_render)
{
	this->mesh_to_render = mesh_to_render;
	owner->aabb.GenerateBoundingBox();
}

ComponentMesh::~ComponentMesh()
{
	App->resources->RemoveResourceFromCacheIfNeeded(mesh_to_render);
}

void ComponentMesh::Delete() 
{
	App->renderer->RemoveComponentMesh(this);
}

void ComponentMesh::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddInt((unsigned int)type, "ComponentType");
	config.AddBool(active, "Active");
	config.AddString(mesh_to_render->exported_file, "MeshPath");
}

void ComponentMesh::Load(const Config& config)
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

}

void ComponentMesh::Render() const
{
	std::string program_name = static_cast<ComponentMaterial*>(owner->GetComponent(ComponentType::MATERIAL))->shader_program;
	GLuint program = App->program->GetShaderProgramId(program_name);
	glUseProgram(program);

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), owner->transform.GetGlobalModelMatrix().Transposed().ptr());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	App->lights->Render(owner->transform.GetGlobalTranslation(), program);
	owner->RenderMaterialTexture(program);
	RenderModel();
	glUseProgram(0);
}	

void ComponentMesh::RenderModel() const
{
	glBindVertexArray(mesh_to_render->GetVAO());
	glDrawElements(GL_TRIANGLES, mesh_to_render->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}