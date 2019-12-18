#include "MeshImporter.h"
#include "Component/ComponentMesh.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>

MeshImporter::MeshImporter()
{

}

bool MeshImporter::Import(const char* file_path, std::string& output_file) const
{

	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == NULL)
	{
		const char *error = aiGetErrorString();
		APP_LOG_ERROR("Error loading model %s ", file_path);
		APP_LOG_ERROR(error);
		return false;
	}

	for (unsigned int i = 0; i < 1; ++i)
	{
		//Get new name
		ModuleFileSystem::File file = ModuleFileSystem::File(file_path);
		if (file.filename.empty())
		{
			APP_LOG_SUCCESS("Importing material error: Couldn't find the file to import.")
				return false;
		}
		output_file = LIBRARY_MESHES_FOLDER + "//" + std::string(scene->mMeshes[i]->mName.C_Str()) + ".ol";
		ImportMesh(scene->mMeshes[i], output_file);
	}
	aiReleaseImport(scene);
	return true;
}

void MeshImporter::ImportMesh(const aiMesh* mesh, const std::string& output_file) const
{
	std::vector<size_t> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	std::vector<ComponentMesh::Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		ComponentMesh::Vertex new_vertex;
		new_vertex.position = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		if (mesh->mTextureCoords[i] != NULL)
		{
			new_vertex.tex_coords = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		vertices.push_back(new_vertex);
	}


	size_t num_indices = indices.size();
	size_t num_vertices = vertices.size();
	size_t ranges[2] = { num_indices, num_vertices };
	size_t size = sizeof(ranges) + sizeof(size_t) * num_indices + sizeof(ComponentMesh::Vertex) * num_vertices;

	char* data = new char[size]; // Allocate
	char* cursor = data;
	size_t bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	cursor += bytes; // Store indices
	bytes = sizeof(size_t) * num_indices;
	memcpy(cursor, &indices, bytes);

	cursor += bytes; // Store vertices
	bytes = sizeof(ComponentMesh::Vertex) * num_vertices;
	memcpy(cursor, &vertices, bytes);

	App->filesystem->Save(output_file.c_str(), data, size);
	delete data;
}

 void MeshImporter::Load(const char* file_path, ComponentMesh & component_mesh) const {
	size_t mesh_size;
	char * data = App->filesystem->Load(file_path, mesh_size);
	char* cursor = data;

	size_t ranges[2];
	//Get ranges
	size_t bytes = sizeof(ranges); // First store ranges
	memcpy(ranges, cursor, bytes);

	component_mesh.indices.reserve(ranges[0]);

	cursor += bytes; // Get indices
	bytes = sizeof(size_t) * ranges[0];
	memcpy(&component_mesh.indices.front(), cursor, bytes);

	component_mesh.vertices.reserve(ranges[1]);

	cursor += bytes; // Get vertices
	bytes = sizeof(ComponentMesh::Vertex) * ranges[1];
	memcpy(&component_mesh.vertices.front(), cursor, bytes);
}