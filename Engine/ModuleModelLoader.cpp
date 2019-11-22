#include "ModuleModelLoader.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleTexture.h"

#include "assimp/DefaultLogger.hpp"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>

ModuleModelLoader::ModuleModelLoader()
{
	
}

// Destructor
ModuleModelLoader::~ModuleModelLoader()
{
}

// Called before render is available
bool ModuleModelLoader::Init()
{
	APP_LOG_SECTION("************ Module ModelLoader Init ************");

	Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Debugging), Assimp::Logger::Debugging);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Info), Assimp::Logger::Info);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Err), Assimp::Logger::Err);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Warn), Assimp::Logger::Warn);

	LoadModel(HOUSE_MODEL_PATH);
	return true;
}

update_status ModuleModelLoader::PreUpdate()
{
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleModelLoader::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleModelLoader::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleModelLoader::CleanUp()
{
	UnloadCurrentModel();
	Assimp::DefaultLogger::kill();
	return true;
}


void ModuleModelLoader::SwapCurrentModel(const char *new_model_file_path)
{
	UnloadCurrentModel();
	LoadModel(new_model_file_path);
}

void ModuleModelLoader::SwapCurrentModelTexture(const char *new_texture_file_path)
{
	APP_LOG_INIT("Swaping current model material texture with texture %s", new_texture_file_path)
	Texture *new_texture = App->texture->loadTexture(new_texture_file_path);
	current_model->SetMaterialTexture(new_texture);
	if (new_texture != nullptr)
	{
		APP_LOG_SUCCESS("Current model texture swaped correctly.")
	}
}


void ModuleModelLoader::UnloadCurrentModel()
{
	delete current_model;
	aiReleaseImport(scene);
}

bool ModuleModelLoader::LoadModel(const char *new_model_file_path)
{
	APP_LOG_INIT("Loading model %s.", new_model_file_path);
	scene = aiImportFile(new_model_file_path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == NULL)
	{
		const char *error = aiGetErrorString();
		APP_LOG_ERROR("Error loading model %s ", new_model_file_path);
		APP_LOG_ERROR(error);
		return false;
	}


	APP_LOG_INFO("Loading model materials.");
	std::string model_base_path = GetModelBasePath(new_model_file_path);
	std::vector<Texture*> material_textures;
	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		APP_LOG_INFO("Loading material %d.", i);
		material_textures.push_back(LoadMaterialData(scene->mMaterials[i], model_base_path));
	}
	APP_LOG_INFO("Model materials loaded correctly.");


	APP_LOG_INFO("Loading model meshes.");
	std::vector<Mesh*> meshes;
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		APP_LOG_INFO("Loading mesh %d", i);
		meshes.push_back(LoadMeshData(scene->mMeshes[i]));
	}
	APP_LOG_INFO("Model meshes loaded correctly.");


	current_model = new Model(meshes, material_textures);
	APP_LOG_INFO("Computing model bounding box.");
	current_model->ComputeBoundingBox();

	App->cameras->Center(*current_model->bounding_box);

	APP_LOG_SUCCESS("Model %s loaded correctly.", new_model_file_path);
	return true;
}

Mesh* ModuleModelLoader::LoadMeshData(const aiMesh *mesh) const
{
	std::vector<Mesh::Vertex> vertices;
	APP_LOG_INFO("Loading mesh vertices");
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Mesh::Vertex vertex;
		vertex.position = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.tex_coords = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertices.push_back(vertex);
	}

	APP_LOG_INFO("Loading mesh indices");
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3);
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	APP_LOG_INFO("Mesh uses material %d", mesh->mMaterialIndex);

	return new Mesh(vertices, indices, mesh->mMaterialIndex);
}

Texture* ModuleModelLoader::LoadMaterialData(const aiMaterial *material, std::string model_base_path)
{
	aiString file;
	aiTextureMapping mapping = aiTextureMapping_UV;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);

	Texture *material_texture;

	APP_LOG_INIT("Loading material texture in described path %s.", file.data);
	material_texture = App->texture->loadTexture(file.data);
	if (material_texture != nullptr)
	{
		APP_LOG_SUCCESS("Material loaded correctly.");
		return material_texture;
	}

	std::string texture_file_name = GetTextureFileName(file.data);

	model_base_path = model_base_path + texture_file_name;
	APP_LOG_INIT("Loading material texture in model folder path %s.", model_base_path.c_str());
	material_texture = App->texture->loadTexture(model_base_path.c_str());
	if (material_texture != nullptr)
	{
		APP_LOG_SUCCESS("Material loaded correctly.");
		return material_texture;
	}

	std::string textures_path = std::string(TEXTURES_PATH) + texture_file_name;
	APP_LOG_INIT("Loading material texture in textures folder %s.", textures_path.c_str());
	material_texture = App->texture->loadTexture(textures_path.c_str());
	if (material_texture != nullptr)
	{
		APP_LOG_SUCCESS("Material loaded correctly.");
		return material_texture;
	}

	return nullptr;
}

std::string ModuleModelLoader::GetModelBasePath(const char *model_file_path) const
{
	std::string file_string = std::string(model_file_path);
	
	std::size_t found = file_string.find_last_of("/\\");
	std::string model_base_path = file_string.substr(0, found + 1);

	return model_base_path;
}

std::string ModuleModelLoader::GetTextureFileName(const char *texture_file_path) const
{
	std::string texture_path_string = std::string(texture_file_path);

	std::size_t found = texture_path_string.find_last_of("/\\");
	if (found == std::string::npos)
	{
		return texture_path_string;
	}
	else
	{
		std::string texture_filename = texture_path_string.substr(found, texture_path_string.length());

		return texture_filename;
	}
}