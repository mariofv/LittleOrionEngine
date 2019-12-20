#include "MeshImporter.h"
#include "Mesh.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include "assimp/DefaultLogger.hpp"

MeshImporter::MeshImporter()
{
	Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Debugging), Assimp::Logger::Debugging);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Info), Assimp::Logger::Info);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Err), Assimp::Logger::Err);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Warn), Assimp::Logger::Warn);
}
MeshImporter::~MeshImporter()
{
	Assimp::DefaultLogger::kill();
}
bool MeshImporter::Import(const char* file_path, std::string& output_file)
{
	loaded_meshes_materials.clear();
	ModuleFileSystem::File file = ModuleFileSystem::File(file_path);
	std::shared_ptr<ModuleFileSystem::File> already_imported = GetAlreadyImportedMesh(file);
	if (already_imported != nullptr) {
		output_file = already_imported->file_path;
		return true;
	}

	output_file = App->filesystem->MakeDirectory(LIBRARY_MESHES_FOLDER, file.filename_no_extension);
	APP_LOG_INIT("Importing model %s.", file_path);

	if (file.filename.empty())
	{
		APP_LOG_SUCCESS("Importing mesh error: Couldn't find the file to import.")
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

	
	aiNode * root_node = scene->mRootNode;
	ImportNode(root_node, scene, output_file);

	aiReleaseImport(scene);
	return true;
}
void MeshImporter::ImportNode(const aiNode * root_node, const aiScene* scene, const std::string& output_file) 
{
	for (size_t i = 0; i < root_node->mNumChildren; i++)
	{
		if (root_node->mChildren[i]->mNumMeshes != 0)
		{
			std::string mesh_file = output_file + "//" + std::string(root_node->mChildren[i]->mName.data) + ".ol";
			size_t mesh_index = root_node->mChildren[i]->mMeshes[0];
			ImportMesh(scene->mMeshes[mesh_index], mesh_file);

			ImportMaterialFromMesh(scene,mesh_index);
		}
		ImportNode(root_node->mChildren[i], scene, output_file);
	}
}
void MeshImporter::ImportMaterialFromMesh(const aiScene* scene, size_t mesh_index)
{
	int mesh_material_index = scene->mMeshes[mesh_index]->mMaterialIndex;
	aiString file;
	aiTextureMapping mapping = aiTextureMapping_UV;
	scene->mMaterials[mesh_material_index]->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);
	loaded_meshes_materials.push_back(std::string(file.data));
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

	std::vector<Mesh::Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Mesh::Vertex new_vertex;
		new_vertex.position = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		new_vertex.tex_coords = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertices.push_back(new_vertex);
	}


	UINT64 num_indices = indices.size();
	UINT64 num_vertices = vertices.size();
	UINT64 ranges[2] = { num_indices, num_vertices };
	UINT64 size = sizeof(ranges) + sizeof(UINT64) * num_indices + sizeof(Mesh::Vertex) * num_vertices;

	char* data = new char[size]; // Allocate
	char* cursor = data;
	UINT64 bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	cursor += bytes; // Store indices
	bytes = sizeof(UINT64) * num_indices;
	memcpy(cursor, &indices.front(), bytes);

	cursor += bytes; // Store vertices
	bytes = sizeof(Mesh::Vertex) * num_vertices;
	memcpy(cursor, &vertices.front(), bytes);

	App->filesystem->Save(output_file.c_str(), data, size);
	delete data;
}

 std::shared_ptr<Mesh> MeshImporter::Load(const char* file_path) {
	
	 //Check if the mesh is already loaded
	 auto it = std::find_if(mesh_cache.begin(), mesh_cache.end(), [file_path](const std::shared_ptr<Mesh> mesh) 
	 {
		 return mesh->mesh_file_path == file_path;
	 });
	 if (it != mesh_cache.end())
	 {
		 APP_LOG_INIT("Model %s exists in cache.", file_path);
		 return *it;
	 }

	APP_LOG_INIT("Loading model %s.", file_path);
	performance_timer.Start();
	size_t mesh_size;
	char * data = App->filesystem->Load(file_path, mesh_size);
	char* cursor = data;

	UINT64 ranges[2];
	//Get ranges
	UINT64 bytes = sizeof(ranges); // First store ranges
	memcpy(ranges, cursor, bytes);

	std::vector<UINT64> indices;
	std::vector<Mesh::Vertex> vertices;

	indices.resize(ranges[0]);

	cursor += bytes; // Get indices
	bytes = sizeof(UINT64) * ranges[0];
	memcpy(&indices.front(), cursor, bytes);

	vertices.resize(ranges[1]);

	cursor += bytes; // Get vertices
	bytes = sizeof(Mesh::Vertex) * ranges[1];
	memcpy(&vertices.front(), cursor, bytes);

	std::shared_ptr<Mesh> new_mesh = std::make_shared<Mesh>(std::move(vertices), std::move(indices), file_path);
	mesh_cache.push_back(new_mesh);
	float time = performance_timer.Read();
	free(data);
	APP_LOG_SUCCESS("Model %s loaded correctly from own format in %f second .", file_path, time);

	return new_mesh;
}

 //Remove the mesh from the cache if the only owner is the cache itself
 void MeshImporter::RemoveMeshFromCacheIfNeeded(std::shared_ptr<Mesh> mesh) {
	 auto it = std::find(mesh_cache.begin(), mesh_cache.end(), mesh);
	 if (it != mesh_cache.end() && (*it).use_count() <= 2)
	 {
		 (*it).~shared_ptr();
	 }
 }

 std::shared_ptr<ModuleFileSystem::File> MeshImporter::GetAlreadyImportedMesh(const ModuleFileSystem::File & file) const{
	 std::vector<std::shared_ptr<ModuleFileSystem::File>> meshes_already_in_library;
	 App->filesystem->GetAllFilesInPath(LIBRARY_MESHES_FOLDER, meshes_already_in_library, true);

	 //Check if the mesh is already loaded
	 auto it = std::find_if(meshes_already_in_library.begin(), meshes_already_in_library.end(), [file](const std::shared_ptr<ModuleFileSystem::File> folder)
	 {
		 return folder->filename.find(file.filename_no_extension) != std::string::npos;
	 });

	 if (it != meshes_already_in_library.end())
	 {
		 return *it;
	 }
	 return nullptr;
 }