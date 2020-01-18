#include "ModuleModelLoader.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleTexture.h"
#include "GameObject.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMaterial.h"
#include "Component/ComponentMesh.h"
#include "Importer/MeshImporter.h"
#include "Importer/MaterialImporter.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>

bool ModuleModelLoader::Init()
{
	APP_LOG_SECTION("************ Module ModelLoader Init ************");
	importing_thread = std::thread(&ModuleModelLoader::StartThread, this);
	return true;
}

bool ModuleModelLoader::CleanUp()
{
	thread_comunication.stop_thread = true;
	importing_thread.join();
	return true;
}

GameObject* ModuleModelLoader::LoadModel(const char *new_model_file_path)
{
	while (thread_comunication.importing_hash == std::hash<std::string>{}(new_model_file_path))
	{
		Sleep(1000);
	}

	File file(new_model_file_path);
	std::string model_output = App->mesh_importer->Import(file).second;

	GameObject *model_root_node = App->scene->CreateGameObject();
	model_root_node->name = std::string(file.filename_no_extension);

	std::vector<std::shared_ptr<File>> files_in_output_path;
	App->filesystem->GetAllFilesInPath(model_output, files_in_output_path);
	for (auto file : files_in_output_path )
	{
		LoadNode(model_root_node, file);
	}
	return model_root_node;
}

void ModuleModelLoader::LoadNode(GameObject *parent_node, const std::shared_ptr<File> & model_base_path)
{
	GameObject *node_game_object = App->scene->CreateChildGameObject(parent_node);

	std::shared_ptr<Mesh> mesh_for_component = App->mesh_importer->Load(model_base_path->file_path.c_str());

	if (mesh_for_component == nullptr)
	{
		return;
	}

	ComponentMesh *mesh_component = (ComponentMesh*)node_game_object->CreateComponent(Component::ComponentType::MESH);
	mesh_component->SetMesh(mesh_for_component);
	node_game_object->name = model_base_path->filename_no_extension;
	node_game_object->Update();

	ComponentMaterial *componentMaterial = (ComponentMaterial*)node_game_object->CreateComponent(Component::ComponentType::MATERIAL);
	for (auto texture : mesh_for_component->meshes_textures_path)
	{
		size_t separator = texture.find_last_of(":");
		std::string texture_path = texture.substr(separator+1, texture.size());
		std::string texture_type = texture.substr(0, separator);
		std::shared_ptr<Texture> texture = App->material_importer->Load(texture_path);
		if (texture.get() != nullptr)
		{
			componentMaterial->SetMaterialTexture(std::stoi(texture_type), App->material_importer->Load(texture_path));
		}
	}
}

GameObject* ModuleModelLoader::LoadCoreModel(const char* new_model_file_path)
{
	File file(new_model_file_path);
	GameObject* model_game_object = App->scene->CreateGameObject();
	model_game_object->name = std::string(file.filename_no_extension);

	std::shared_ptr<Mesh> mesh_for_component = App->mesh_importer->Load(file.file_path.c_str());
	if (mesh_for_component == nullptr)
	{
		return model_game_object;
	}
	ComponentMesh* mesh_component = (ComponentMesh*)model_game_object->CreateComponent(Component::ComponentType::MESH);
	mesh_component->SetMesh(mesh_for_component);
	model_game_object->Update();

	ComponentMaterial* componentMaterial = (ComponentMaterial*)model_game_object->CreateComponent(Component::ComponentType::MATERIAL);
	for (auto texture : mesh_for_component->meshes_textures_path)
	{
		size_t separator = texture.find_last_of(":");
		std::string texture_path = texture.substr(separator + 1, texture.size());
		std::string texture_type = texture.substr(0, separator);
		componentMaterial->SetMaterialTexture(std::stoi(texture_type), App->material_importer->Load(texture_path));
	}
	return model_game_object;
}


// TODO: Change this method to a proper class
void ModuleModelLoader::StartThread()
{
	thread_comunication.finished_loading = false;
	thread_comunication.total_items = App->filesystem->assets_file->total_sub_files_number;
	Import(*App->filesystem->assets_file.get());
	thread_comunication.finished_loading = true;
}

void ModuleModelLoader::Import(const File& file)
{
	for (auto & child : file.children)
	{
		if (thread_comunication.stop_thread)
		{
			return;
		}
		thread_comunication.importing_hash = std::hash<std::string>{}(child->file_path);
		if (child->file_type == FileType::MODEL)
		{
			App->mesh_importer->Import(*child.get());
		}
		if (child->file_type == FileType::TEXTURE)
		{
			App->material_importer->Import(*child.get());
		}
		if (child->file_type == FileType::DIRECTORY)
		{
			Import(*child.get());
		}
		++thread_comunication.loaded_items;
		thread_comunication.importing_hash = 0;
	}
}