#include "ComponentMesh.h"
#include "Application.h"
#include "Module/ModuleRender.h"
#include "Importer/MeshImporter.h"
#include "UI/ComponentsUI.h"

ComponentMesh::ComponentMesh(std::shared_ptr<Mesh> mesh_to_render) : mesh_to_render(mesh_to_render), Component(nullptr, ComponentType::MESH)
{
	SetupMesh();
}

ComponentMesh::ComponentMesh(std::shared_ptr<Mesh> mesh_to_render, GameObject * owner) : mesh_to_render(mesh_to_render), Component(owner, ComponentType::MESH)
{
	SetupMesh();
}

ComponentMesh::ComponentMesh() : Component(nullptr, ComponentType::MESH)
{

}

void ComponentMesh::SetMesh(std::shared_ptr<Mesh> mesh_to_render)
{
	this->mesh_to_render = mesh_to_render;
	SetupMesh();
}

ComponentMesh::~ComponentMesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
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
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, mesh_to_render->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ComponentMesh::SetupMesh()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, mesh_to_render->vertices.size() * sizeof(Mesh::Vertex), &mesh_to_render->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_to_render->indices.size() * sizeof(uint32_t), &mesh_to_render->indices[0], GL_STATIC_DRAW);

	// VERTEX POSITION
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)0);

	// VERTEX TEXTURE COORDS
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, tex_coords));

	// VERTEX NORMALS
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, normals));

	glBindVertexArray(0);
}

void ComponentMesh::ShowComponentWindow()
{
	ComponentsUI::ShowComponentMeshWindow(this);
}