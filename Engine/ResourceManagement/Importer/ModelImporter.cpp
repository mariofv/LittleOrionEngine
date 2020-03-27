#include "ModelImporter.h"

#include "Helper/Config.h"

#include "Filesystem/File.h"
#include "Filesystem/Path.h"	

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/Importer/MaterialImporter.h"
#include "ResourceManagement/Importer/ModelImporters/AnimationImporter.h"
#include "ResourceManagement/Importer/ModelImporters/MeshImporter.h"
#include "ResourceManagement/Importer/ModelImporters/SkeletonImporter.h"
#include "ResourceManagement/Importer/PrefabImporter.h"
#include "ResourceManagement/Resources/Mesh.h"
#include "ResourceManagement/Resources/Material.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/DefaultLogger.hpp>

#include <Brofiler/Brofiler.h>

ModelImporter::ModelImporter() : Importer(ResourceType::MODEL)
{
	Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Debugging), Assimp::Logger::Debugging);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Info), Assimp::Logger::Info);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Err), Assimp::Logger::Err);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Warn), Assimp::Logger::Warn);
}

ModelImporter::~ModelImporter()
{
	Assimp::DefaultLogger::kill();
}

FileData ModelImporter::ExtractData(Path& assets_file_path) const
{
	FileData model_data;

	APP_LOG_INIT("Importing model %s.", assets_file_path.GetFullPath().c_str())

	performance_timer.Start();
	FileData file_data = assets_file_path.GetFile()->Load();
	const aiScene* scene = aiImportFileFromMemory((char*)file_data.buffer, file_data.size, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	if (scene == NULL)
	{
		const char *error = aiGetErrorString();
		APP_LOG_ERROR("Error loading model %s ", assets_file_path.GetFullPath().c_str());
		APP_LOG_ERROR(error);
		return model_data;
	}
	performance_timer.Stop();
	float time = performance_timer.Read();

	APP_LOG_SUCCESS("Model %s loaded correctly from assimp in %f ms.", assets_file_path.GetFullPath().c_str(), time);


	aiNode * root_node = scene->mRootNode;
	Path* asset_file_folder_path = assets_file_path.GetParent();
	aiMatrix4x4 identity_transformation = aiMatrix4x4();
	std::vector<Config> node_config = ExtractDataFromNode(root_node, identity_transformation, scene, *asset_file_folder_path);

	std::vector<Config> animations_config;
	for (size_t i = 0; i < scene->mNumAnimations; i++)
	{
		//Import animation
		Config animation_config;
		std::string animation_name = scene->mAnimations[i]->mName.C_Str();
		uint32_t extracted_animation_uuid = ExtractAnimationFromNode(scene, scene->mAnimations[i], animation_name, *asset_file_folder_path);

		animation_config.AddUInt(extracted_animation_uuid, "Animation");
		animations_config.push_back(animation_config);
	}
	aiReleaseImport(scene);

	Config model;
	model.AddString(assets_file_path.GetFilenameWithoutExtension(), "Name");
	model.AddChildrenConfig(node_config, "Node");
	model.AddChildrenConfig(animations_config, "Animations");

	model_data = App->resources->prefab_importer->ExtractFromModel(model);
	return model_data;
}

std::vector<Config> ModelImporter::ExtractDataFromNode(const aiNode* root_node, const aiMatrix4x4& parent_transformation, const aiScene* scene, Path& asset_file_folder_path) const
{
	std::vector<Config> node_config;

	aiMatrix4x4& current_transformation = parent_transformation * root_node->mTransformation;

	for (size_t i = 0; i < root_node->mNumMeshes; ++i)
	{
		Config node;
		size_t mesh_index = root_node->mMeshes[i];
		aiMesh* node_mesh = scene->mMeshes[mesh_index];
		std::string mesh_name = std::string(node_mesh->mName.data) + "_" + std::to_string(i);
		node.AddString(mesh_name, "Name");

		uint32_t extracted_material_uuid = ExtractMaterialFromNode(scene, mesh_index, mesh_name, asset_file_folder_path);
		node.AddUInt(extracted_material_uuid, "Material");

		uint32_t extracted_mesh_uuid = ExtractMeshFromNode(node_mesh, mesh_name, parent_transformation, asset_file_folder_path);
		if (extracted_mesh_uuid != 0)
		{
			node.AddUInt(extracted_mesh_uuid, "Mesh");
		}

		if (node_mesh->HasBones())
		{
			uint32_t extracted_skeleton_uuid = ExtractSkeletonFromNode(scene, node_mesh, mesh_name, asset_file_folder_path);
			node.AddUInt(extracted_skeleton_uuid, "Skeleton");
		}
		node_config.push_back(node);
	}

	for (size_t i = 0; i < root_node->mNumChildren; i++)
	{
		std::vector<Config> child_node_config = ExtractDataFromNode(root_node->mChildren[i], current_transformation, scene, asset_file_folder_path);
		node_config.insert(node_config.begin(), child_node_config.begin(), child_node_config.end());
	}

	return node_config;
}

uint32_t ModelImporter::ExtractMaterialFromNode(const aiScene* scene, size_t mesh_index, const std::string& mesh_name, Path& asset_file_folder_path) const
{
	int mesh_material_index = scene->mMeshes[mesh_index]->mMaterialIndex;
	aiMaterial* assimp_mesh_material = scene->mMaterials[mesh_material_index];
	FileData mesh_material_data = App->resources->material_importer->ExtractMaterialFromAssimp(assimp_mesh_material, asset_file_folder_path);
	uint32_t extracted_material_uuid = App->resources->CreateFromData(mesh_material_data, asset_file_folder_path, mesh_name + ".mat");
	return extracted_material_uuid;
}

uint32_t ModelImporter::ExtractMeshFromNode(const aiMesh* asssimp_mesh, std::string mesh_name, const aiMatrix4x4& mesh_transformation, Path& asset_file_folder_path) const
{
	FileData mesh_data = App->resources->mesh_importer->ExtractMeshFromAssimp(asssimp_mesh, mesh_transformation);
	if (mesh_data.size == 0)
	{
		return 0;
	}

	uint32_t extracted_mesh_uuid = App->resources->CreateFromData(mesh_data, asset_file_folder_path, mesh_name + ".mesh");
	return extracted_mesh_uuid;
}

uint32_t ModelImporter::ExtractSkeletonFromNode(const aiScene* scene, const aiMesh* asssimp_mesh, std::string mesh_name, Path& asset_file_folder_path) const
{
	FileData mesh_data = App->resources->skeleton_importer->ExtractSkeletonFromAssimp(scene, asssimp_mesh);
	uint32_t extracted_skeleton_uuid = App->resources->CreateFromData(mesh_data, asset_file_folder_path, mesh_name + "_skeleton.sk");
	return extracted_skeleton_uuid;
}

uint32_t ModelImporter::ExtractAnimationFromNode(const aiScene* scene, const aiAnimation* assimp_animation, std::string animation_name, Path& asset_file_folder_path) const
{
	FileData animation_data = App->resources->animation_importer->ExtractAnimationFromAssimp(scene, assimp_animation);
	uint32_t extracted_animation_uuid = App->resources->CreateFromData(animation_data, asset_file_folder_path, animation_name + ".anim");
	return extracted_animation_uuid;
}