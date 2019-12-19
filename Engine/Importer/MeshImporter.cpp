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

bool MeshImporter::Import(const char* file_path, std::string& output_file)
{
	APP_LOG_INIT("Importing model %s.", file_path);

	ModuleFileSystem::File file = ModuleFileSystem::File(file_path);
	if (file.filename.empty())
	{
		APP_LOG_SUCCESS("Importing material error: Couldn't find the file to import.")
			return false;
	}
	performance_timer.Start();

	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == NULL)
	{
		const char *error = aiGetErrorString();
		APP_LOG_ERROR("Error loading model %s ", file_path);
		APP_LOG_ERROR(error);
		return false;
	}

	float time = performance_timer.Read();
	APP_LOG_SUCCESS("Model %s loaded correctly from assimp in %f second .", file_path, time);

	std::string filename_no_extension = file.filename.substr(0, file.filename.find_last_of("."));
	if (scene->mNumMeshes == 1) {
		output_file = LIBRARY_MESHES_FOLDER + "//" + std::string(filename_no_extension) + ".ol";
		ImportMesh(scene->mMeshes[0], output_file);
	}
	else 
	{

		 output_file = LIBRARY_MESHES_FOLDER + "//" + std::string(scene->mRootNode->mChildren[0]->mName.data) + ".ol";
		ImportMesh(scene->mMeshes[0], output_file);
		/*
		aiNode * root_node = scene->mRootNode;
		App->filesystem->MakeDirectory(LIBRARY_MESHES_FOLDER, filename_no_extension);
		//std::string output_file = LIBRARY_MESHES_FOLDER + "//" + filename_no_extension;
		for (UINT64 i = 0; i < root_node->mNumChildren; i++)
		{
			std::string output_file = LIBRARY_MESHES_FOLDER + "//" + std::string(scene->mRootNode->mChildren[i]->mName.data) + ".ol";
			ImportMesh(scene->mMeshes[i], output_file);
		}*/
	}
	aiReleaseImport(scene);
	return true;
}


void MeshImporter::ImportMesh(const aiMesh* mesh, const std::string& output_file) const
{
	std::vector<UINT64> indices;
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


	UINT64 num_indices = indices.size();
	UINT64 num_vertices = vertices.size();
	UINT64 ranges[2] = { num_indices, num_vertices };
	UINT64 size = sizeof(ranges) + sizeof(UINT64) * num_indices + sizeof(ComponentMesh::Vertex) * num_vertices;

	char* data = new char[size]; // Allocate
	char* cursor = data;
	UINT64 bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	cursor += bytes; // Store indices
	bytes = sizeof(UINT64) * num_indices;
	memcpy(cursor, &indices.front(), bytes);

	cursor += bytes; // Store vertices
	bytes = sizeof(ComponentMesh::Vertex) * num_vertices;
	memcpy(cursor, &vertices.front(), bytes);

	App->filesystem->Save(output_file.c_str(), data, size);
	delete data;
}

 void MeshImporter::Load(const char* file_path, ComponentMesh & component_mesh) {

	APP_LOG_INIT("Loading model %s.", file_path);
	performance_timer.Start();
	size_t mesh_size;
	char * data = App->filesystem->Load(file_path, mesh_size);
	char* cursor = data;

	UINT64 ranges[2];
	//Get ranges
	UINT64 bytes = sizeof(ranges); // First store ranges
	memcpy(ranges, cursor, bytes);

	component_mesh.indices.resize(ranges[0]);

	cursor += bytes; // Get indices
	bytes = sizeof(UINT64) * ranges[0];
	memcpy(&component_mesh.indices.front(), cursor, bytes);

	component_mesh.vertices.resize(ranges[1]);

	cursor += bytes; // Get vertices
	bytes = sizeof(ComponentMesh::Vertex) * ranges[1];
	memcpy(&component_mesh.vertices.front(), cursor, bytes);

	component_mesh.LoadMesh();
	float time = performance_timer.Read();
	free(data);
	APP_LOG_SUCCESS("Model %s loaded correctly from own format in %f second .", file_path, time);
}