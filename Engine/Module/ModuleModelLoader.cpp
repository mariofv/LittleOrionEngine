#include "ModuleModelLoader.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleTexture.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMaterial.h"
#include "Component/ComponentMesh.h"
#include "Importer/MeshImporter.h"

#include "assimp/DefaultLogger.hpp"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>

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

// Called before quitting
bool ModuleModelLoader::CleanUp()
{
	Assimp::DefaultLogger::kill();
	return true;
}


GameObject* ModuleModelLoader::LoadModel(const char *new_model_file_path)
{
	std::string model_output;

	App->mesh_importer->Import(new_model_file_path, model_output);

	ModuleFileSystem::File file = ModuleFileSystem::File(new_model_file_path);
	GameObject *model_root_node = App->scene->CreateGameObject();
	model_root_node->name = std::string("RootNode");

	model_output = model_output.substr(0, model_output.size());
	std::vector<std::shared_ptr<ModuleFileSystem::File>> files_in_output_path;
	App->filesystem->GetAllFilesInPath(model_output, files_in_output_path);
	for (auto file : files_in_output_path )
	{
		LoadNode(model_root_node, file->file_path);
	}

	return model_root_node;
}

void ModuleModelLoader::LoadNode(GameObject *parent_node, const std::string model_base_path)
{
	GameObject *node_game_object = App->scene->CreateChildGameObject(parent_node);

	std::shared_ptr<Mesh> mesh_for_component = App->mesh_importer->Load(model_base_path.c_str());
	ComponentMesh *mesh_component = (ComponentMesh*)node_game_object->CreateComponent(Component::ComponentType::MESH);
	mesh_component->SetMesh(mesh_for_component);
	node_game_object->Update();
}

std::shared_ptr<Texture> ModuleModelLoader::LoadMaterialData(const aiMaterial *material, const std::string model_base_path)
{
	aiString file;
	aiTextureMapping mapping = aiTextureMapping_UV;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);

	std::shared_ptr<Texture> material_texture;

	APP_LOG_INIT("Loading material texture in described path %s.", file.data);
	material_texture = App->texture->LoadTexture(file.data);
	if (material_texture != nullptr)
	{
		APP_LOG_SUCCESS("Material loaded correctly.");
		return material_texture;
	}

	std::string texture_file_name = GetTextureFileName(file.data);

	std::string textures_path = model_base_path + texture_file_name;
	APP_LOG_INIT("Loading material texture in model folder path %s.", model_base_path.c_str());
	material_texture = App->texture->LoadTexture(textures_path.c_str());
	if (material_texture != nullptr)
	{
		APP_LOG_SUCCESS("Material loaded correctly.");
		return material_texture;
	}

	textures_path = std::string(TEXTURES_PATH) + texture_file_name;
	APP_LOG_INIT("Loading material texture in textures folder %s.", textures_path.c_str());
	material_texture = App->texture->LoadTexture(textures_path.c_str());
	if (material_texture != nullptr)
	{
		APP_LOG_SUCCESS("Material loaded correctly.");
		return material_texture;
	}

	return nullptr;
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
