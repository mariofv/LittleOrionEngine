#include "MeshImporter.h"
#include <assimp/mesh.h>
#include "Main/Application.h"
#include <ResourceManagement/Resources/Mesh.h>
#include "Module/ModuleFileSystem.h"

void MeshImporter::ImportMesh(const aiMesh* mesh, const std::vector<std::string> & loaded_meshes_materials, const aiMatrix4x4& mesh_transformation, const std::string& output_file) const
{
	std::vector<uint32_t> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//We only accept triangle formed meshes
	if (indices.size() % 3 != 0)
	{
		APP_LOG_ERROR("Mesh %s have incorrect indices", mesh->mName.C_Str());
		return;
	}
	std::vector<Mesh::Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Mesh::Vertex new_vertex;
		aiVector3D transformed_position = mesh_transformation * mesh->mVertices[i];
		new_vertex.position = float3(transformed_position.x, transformed_position.y, transformed_position.z);
		new_vertex.tex_coords = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		new_vertex.normals = float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		vertices.push_back(new_vertex);
	}

	std::vector<uint32_t> materials_path_size;
	uint32_t total_meshes_size = 0;
	for (auto & path_size : loaded_meshes_materials)
	{
		materials_path_size.push_back(path_size.size());
		total_meshes_size += path_size.size();
	}

	uint32_t num_indices = indices.size();
	uint32_t num_vertices = vertices.size();
	uint32_t num_materials = loaded_meshes_materials.size();
	uint32_t ranges[3] = { num_indices, num_vertices, num_materials };

	uint32_t size = sizeof(ranges) + sizeof(uint32_t) * num_indices + sizeof(Mesh::Vertex) * num_vertices + sizeof(uint32_t) * num_materials + total_meshes_size;

	char* data = new char[size]; // Allocate
	char* cursor = data;
	size_t bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	cursor += bytes; // Store indices
	bytes = sizeof(uint32_t) * num_indices;
	memcpy(cursor, &indices.front(), bytes);

	cursor += bytes; // Store vertices
	bytes = sizeof(Mesh::Vertex) * num_vertices;
	memcpy(cursor, &vertices.front(), bytes);

	cursor += bytes; // Store sizes
	bytes = sizeof(uint32_t) * num_materials;
	if (bytes != 0)
	{
		memcpy(cursor, &materials_path_size.front(), bytes);
	}
	for (size_t i = 0; i < num_materials; i++)
	{
		cursor += bytes; // Store materials
		bytes = materials_path_size.at(i);
		memcpy(cursor, loaded_meshes_materials[i].c_str(), bytes);
	}


	App->filesystem->Save(output_file.c_str(), data, size);
	delete data;
}