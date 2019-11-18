#include "Model.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"

#include <limits>       // std::numeric_limits
#include <algorithm>    // std::max

Model::Model(const std::vector<Mesh*> meshes, const unsigned int num_materials, GLuint* material_textures)
{
	this->meshes = meshes;
	this->material_textures = material_textures;
	this->num_materials = num_materials;
}


Model::~Model()
{
	glDeleteTextures(num_materials, material_textures);
	delete[] material_textures;

	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		delete meshes[i];
	}
	meshes.clear();

	delete bounding_box;
}


void Model::Render(GLuint shader_program) const
{
	glUseProgram(shader_program);

	// CREATES MODEL MATRIX
	float4x4 model = float4x4::FromTRS(
		float3(0.0f, 0.0f, 0.0f),
		float3x3::identity,
		float3(1.0f, 1.0f, 1.0f)
	);

	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "model"),
		1,
		GL_TRUE,
		&model[0][0]
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
		GLuint mesh_texture = material_textures[meshes[i]->material_index];
		meshes[i]->Render(shader_program, mesh_texture);
	}

	glUseProgram(0);


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