#include "MeshImporter.h"


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

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
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
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	size_t num_indices = indices.size();
	size_t num_vertices = mesh->mNumVertices;
	size_t ranges[2] = { num_indices, num_vertices };
	size_t size = sizeof(ranges) + sizeof(size_t) * num_indices + sizeof(float) * num_vertices * 3;

	char* data = new char[size]; // Allocate
	char* cursor = data;
	size_t bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	cursor += bytes; // Store indices
	bytes = sizeof(size_t) * num_indices;
	memcpy(cursor, &indices, bytes);

	cursor += bytes; // Store vertices
	bytes = sizeof(size_t) * num_vertices;
	memcpy(cursor, mesh->mVertices, bytes);

	App->filesystem->Save(output_file.c_str(), cursor, size);
}