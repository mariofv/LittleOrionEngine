#include "ComponentMesh.h"
#include "Application.h"
#include "Module/ModuleRender.h"
#include "Importer/MeshImporter.h"
#include "UI/ComponentsUI.h"

ComponentMesh::ComponentMesh(const std::shared_ptr<Mesh> & mesh_to_render) : mesh_to_render(mesh_to_render), Component(nullptr, ComponentType::MESH)
{
}

ComponentMesh::ComponentMesh(const std::shared_ptr<Mesh> & mesh_to_render, GameObject * owner) : mesh_to_render(mesh_to_render), Component(owner, ComponentType::MESH)
{
}

ComponentMesh::ComponentMesh() : Component(nullptr, ComponentType::MESH)
{
}

void ComponentMesh::SetMesh(const std::shared_ptr<Mesh> & mesh_to_render)
{
	this->mesh_to_render = mesh_to_render;
}

ComponentMesh::~ComponentMesh()
{
	App->mesh_importer->RemoveMeshFromCacheIfNeeded(mesh_to_render);
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
	config.AddString(mesh_to_render->mesh_file_path, "MeshPath");
}

void ComponentMesh::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);

	std::string mesh_path;
	config.GetString("MeshPath", mesh_path, "");
	SetMesh(App->mesh_importer->Load(mesh_path.c_str()));
}

void ComponentMesh::Render() const
{
	glBindVertexArray(mesh_to_render->GetVAO());
	glDrawElements(GL_TRIANGLES, mesh_to_render->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ComponentMesh::ShowComponentWindow()
{
	ComponentsUI::ShowComponentMeshWindow(this);
}