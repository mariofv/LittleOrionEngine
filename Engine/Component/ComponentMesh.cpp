#include "ComponentMesh.h"
#include "Application.h"
#include "Module/ModuleRender.h"

ComponentMesh::ComponentMesh() : Component(nullptr, ComponentType::MESH)
{

}

ComponentMesh::ComponentMesh(GameObject * owner) : Component(owner, ComponentType::MESH)
{

}

ComponentMesh::~ComponentMesh()
{

	vertices.clear();
	indices.clear();

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void ComponentMesh::Enable()
{

}

void ComponentMesh::Disable()
{

}

void ComponentMesh::Update()
{

}

void ComponentMesh::Delete() 
{
	App->renderer->RemoveComponentMesh(this);
}

void ComponentMesh::LoadMesh(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices, const unsigned int material_index)
{
	this->vertices = vertices;
	this->indices = indices;
	this->material_index = material_index;

	num_vertices = vertices.size();
	num_triangles = num_vertices / 3;

	SetupMesh();
}

void ComponentMesh::Render() const
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ComponentMesh::SetupMesh()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// VERTEX POSITION
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// VERTEX TEXTURE COORDS
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

	glBindVertexArray(0);
}

void ComponentMesh::ShowComponentWindow()
{
	ComponentsUI::ShowComponentMeshWindow(this);
}