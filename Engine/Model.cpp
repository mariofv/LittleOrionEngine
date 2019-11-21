#include "Model.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"

#include "imgui.h"
#include "IconsFontAwesome5.h"

#include <limits>       // std::numeric_limits
#include <algorithm>    // std::max

Model::Model(const std::vector<Mesh*> meshes, const std::vector<Texture*> material_textures)
{
	this->meshes = meshes;
	this->material_textures = material_textures;
	num_materials = material_textures.size();

	translation = float3::zero;
	rotation = float3::zero,
	scale = float3::one;

	num_vertices = 0;
	num_triangles = 0;
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		num_vertices += meshes[i]->vertices.size();
		num_triangles += (meshes[i]->indices.size() / 3.f);
	}
}


Model::~Model()
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		delete meshes[i];
	}
	meshes.clear();

	for (unsigned int i = 0; i < material_textures.size(); ++i)
	{
		delete material_textures[i];
	}
	meshes.clear();

	delete bounding_box;
}


void Model::Render(GLuint shader_program) const
{
	float4x4 model_matrix = float4x4::FromTRS(
		translation, 
		float3x3::FromEulerXYZ(rotation.x,rotation.y,rotation.z),
		scale
	);

	glUseProgram(shader_program);

	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "model"),
		1,
		GL_TRUE,
		&model_matrix[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "view"),
		1,
		GL_TRUE,
		&App->cameras->view[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "proj"),
		1,
		GL_TRUE,
		&App->cameras->proj[0][0]
	);


	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		GLuint mesh_texture;
		if (material_textures[meshes[i]->material_index] != nullptr)
		{
			mesh_texture = material_textures[meshes[i]->material_index]->opengl_texture;
		}
		else
		{
			mesh_texture = 0;
		}
		meshes[i]->Render(shader_program, mesh_texture);
	}

	glUseProgram(0);


}

void Model::SetMaterialTexture(Texture* new_texture)
{
	delete material_textures[0];
	material_textures[0] = new_texture;
}

void Model::ComputeBoundingBox()
{
	const float infinite = std::numeric_limits<float>::infinity();

	float3 min_coordinates = float3(infinite);
	float3 max_coordinates = float3(-infinite);

	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		for (unsigned int j = 0; j < meshes[i]->vertices.size(); ++j)
		{
			if (meshes[i]->vertices[j].position.x > max_coordinates.x)
			{
				max_coordinates.x = meshes[i]->vertices[j].position.x;
			}
			if (meshes[i]->vertices[j].position.x < min_coordinates.x)
			{
				min_coordinates.x = meshes[i]->vertices[j].position.x;
			}


			if (meshes[i]->vertices[j].position.y > max_coordinates.y)
			{
				max_coordinates.y = meshes[i]->vertices[j].position.y;
			}
			if (meshes[i]->vertices[j].position.y < min_coordinates.y)
			{
				min_coordinates.y = meshes[i]->vertices[j].position.y;
			}


			if (meshes[i]->vertices[j].position.z > max_coordinates.z)
			{
				max_coordinates.z = meshes[i]->vertices[j].position.z;
			}
			if (meshes[i]->vertices[j].position.z < min_coordinates.z)
			{
				min_coordinates.z = meshes[i]->vertices[j].position.z;
			}

		}
	}

	bounding_box = new BoundingBox(min_coordinates, max_coordinates);
}

void Model::ShowModelProperties()
{
	if (ImGui::Begin(ICON_FA_CUBE" Properties"))
	{
		if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED " Transform"))
		{	
			ImGui::DragFloat3("Translation", &translation[0], NULL, NULL, NULL);
			ImGui::DragFloat3("Rotation", &rotation[0], NULL, NULL, NULL);
			ImGui::DragFloat3("Scale", &scale[0], NULL, NULL, NULL);
		}

		ImGui::Spacing();

		if (ImGui::CollapsingHeader(ICON_FA_SHAPES " Geometry")) {
		
			int tmp_num_meshes = meshes.size();

			ImGui::DragInt("# Meshes", &tmp_num_meshes, NULL, NULL, NULL);
			ImGui::DragInt("# Triangles", &num_triangles, NULL, NULL, NULL);
			ImGui::DragInt("# Vertices", &num_vertices, NULL, NULL, NULL);
		}

		ImGui::Spacing();

		if (ImGui::CollapsingHeader(ICON_FA_IMAGE" Texture"))
		{
			char tmp_string[256];
			int window_width = ImGui::GetWindowWidth();

			for (unsigned int i = 0; i < num_materials; ++i)
			{
				sprintf_s(tmp_string, "Material %d", i);
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::BeginChild(tmp_string, ImVec2(ImGui::GetWindowWidth()*0.9, 0), true, ImGuiWindowFlags_MenuBar);
				if (ImGui::BeginMenuBar())
				{
					ImGui::BeginMenu(tmp_string, false);
					ImGui::EndMenuBar();
				}
				
				ImGui::Text("Texture path:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &material_textures[i]->texture_path[0]);

				sprintf_s(tmp_string, "%dx%d px", material_textures[i]->width, material_textures[i]->height);
				ImGui::Text("Texture size:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

				ImGui::Text("Mipmap:");
				ImGui::SameLine();
				if (material_textures[i]->IsMipMapped())
				{
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Yes");
				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "No");
				}
					
				ImGui::Text("Wrap S:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &material_textures[i]->GetWrapS_C_Str()[0]);

				ImGui::Text("Wrap T:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &material_textures[i]->GetWrapT_C_Str()[0]);

				ImGui::Text("Min filter:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &material_textures[i]->GetMinFilter_C_Str()[0]);

				ImGui::Text("Mag filter:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &material_textures[i]->GetMagFilter_C_Str()[0]);

				ImGui::Spacing();

				ImGui::SetCursorPosX(window_width*0.25f);
				ImGui::Image((void*)(intptr_t)material_textures[0]->opengl_texture, ImVec2(window_width * 0.5f, window_width * 0.5f), ImVec2(0, 1), ImVec2(1, 0));
					
				ImGui::EndChild();
				ImGui::PopStyleVar();
			}
			
		}

		ImGui::End();
	}
}