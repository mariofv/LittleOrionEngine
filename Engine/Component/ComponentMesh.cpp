#include "ComponentMesh.h"

#include "imgui.h"
#include "IconsFontAwesome5.h"

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

Component::ComponentType ComponentMesh::GetType() const
{
	return Component::ComponentType::MESH;
}

void ComponentMesh::LoadMesh(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices, const unsigned int material_index)
{
	this->vertices = vertices;
	this->indices = indices;
	this->material_index = material_index;

	GenerateBoundingBox();

	num_vertices = vertices.size();
	num_triangles = num_vertices / 3;

	SetupMesh();
}

void ComponentMesh::Render(const GLuint shader_program, const GLuint texture) const
{
	if (active)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shader_program, "texture0"), 0);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
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

void ComponentMesh::GenerateBoundingBox()
{
	bounding_box.SetNegativeInfinity();
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		bounding_box.Enclose(vertices[i].position);
	}
}

void ComponentMesh::ShowComponentWindow()
{
	if (ImGui::CollapsingHeader(ICON_FA_SHAPES " Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &active);
		ImGui::Separator();

		ImGui::DragInt("# Triangles", &num_triangles, NULL, NULL, NULL);
		ImGui::DragInt("# Vertices", &num_vertices, NULL, NULL, NULL);
	}
}