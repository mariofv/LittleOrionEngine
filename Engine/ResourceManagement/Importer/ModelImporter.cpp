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

FileData ModelImporter::ExtractData(Path& assets_file_path, const Metafile& metafile) const
{
	FileData model_data{NULL, 0};

	// LOAD ASSIMP SCENE
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


	// LOCATE PATH OF MODEL FILE
	Path* asset_file_folder_path = assets_file_path.GetParent();


	// LOAD NATIVE SCALE
	float unit_scale_factor = 1.f;
	for (unsigned int i = 0; i < scene->mMetaData->mNumProperties; ++i)
	{
		if (scene->mMetaData->mKeys[i] == aiString("UnitScaleFactor"))
		{
			aiMetadataEntry unit_scale_entry = scene->mMetaData->mValues[i];
			unit_scale_factor = *(double*)unit_scale_entry.mData;
		};
	}
	unit_scale_factor *= 0.01f;


	// INIT SKELETON CACHE
	std::map<std::string, uint32_t> skeleton_cache;


	// STORE ALL MODEL GLOBAL DATA
	current_model_data = {scene, asset_file_folder_path, unit_scale_factor, skeleton_cache};


	aiNode* root_node = scene->mRootNode;
	aiMatrix4x4 identity_transformation = aiMatrix4x4();
	std::vector<Config> node_config = ExtractDataFromNode(root_node, identity_transformation);

	std::vector<Config> animations_config;
	for (size_t i = 0; i < scene->mNumAnimations; i++)
	{
		//Import animation
		Config animation_config;
		std::string animation_name = assets_file_path.GetFilenameWithoutExtension() +"_"+ scene->mAnimations[i]->mName.C_Str();
		uint32_t extracted_animation_uuid = ExtractAnimationFromNode(scene->mAnimations[i], animation_name);

		animation_config.AddUInt(extracted_animation_uuid, "Animation");
		animations_config.push_back(animation_config);
	}
	aiReleaseImport(scene);

	Config model;
	model.AddString(assets_file_path.GetFilenameWithoutExtension(), "Name");
	model.AddChildrenConfig(node_config, "Node");
	model.AddChildrenConfig(animations_config, "Animations");

	model_data = App->resources->prefab_importer->ExtractFromModel(model, metafile);
	return model_data;
}

std::vector<Config> ModelImporter::ExtractDataFromNode(const aiNode* root_node, const aiMatrix4x4& parent_transformation) const
{
	std::vector<Config> node_config;

	aiMatrix4x4& current_transformation = parent_transformation * root_node->mTransformation;
	// Transformation

	std::map<std::string, std::shared_ptr<Skeleton>> already_loaded_skeleton;
	for (size_t i = 0; i < root_node->mNumMeshes; ++i)
	{
		Config node;
		size_t mesh_index = root_node->mMeshes[i];
		aiMesh* node_mesh = current_model_data.scene->mMeshes[mesh_index];
		std::string mesh_name = std::string(node_mesh->mName.data) + "_" + std::to_string(i);
		node.AddString(mesh_name, "Name");

		uint32_t extracted_material_uuid = ExtractMaterialFromNode(mesh_index, mesh_name);
		node.AddUInt(extracted_material_uuid, "Material");
		
		uint32_t extracted_skeleton_uuid = 0;
		if (node_mesh->HasBones())
		{
			extracted_skeleton_uuid = ExtractSkeletonFromNode(node_mesh, mesh_name);
			if (extracted_skeleton_uuid != 0)
			{
				node.AddUInt(extracted_skeleton_uuid, "Skeleton");
			}
		}

		uint32_t extracted_mesh_uuid = ExtractMeshFromNode(node_mesh, mesh_name, parent_transformation, extracted_skeleton_uuid);
		if (extracted_mesh_uuid != 0)
		{
			node.AddUInt(extracted_mesh_uuid, "Mesh");
		}

		node_config.push_back(node);
	}

	for (size_t i = 0; i < root_node->mNumChildren; i++)
	{
		std::vector<Config> child_node_config = ExtractDataFromNode(root_node->mChildren[i], current_transformation);
		node_config.insert(node_config.begin(), child_node_config.begin(), child_node_config.end());
	}

	return node_config;
}

uint32_t ModelImporter::ExtractMaterialFromNode(size_t mesh_index, const std::string& mesh_name) const
{
	int mesh_material_index = current_model_data.scene->mMeshes[mesh_index]->mMaterialIndex;
	aiMaterial* assimp_mesh_material = current_model_data.scene->mMaterials[mesh_material_index];
	FileData mesh_material_data = App->resources->material_importer->ExtractMaterialFromAssimp(assimp_mesh_material, *current_model_data.asset_file_folder_path);
	uint32_t extracted_material_uuid = App->resources->CreateFromData(mesh_material_data, *current_model_data.asset_file_folder_path, mesh_name + ".mat");
	return extracted_material_uuid;
}

uint32_t ModelImporter::ExtractMeshFromNode(const aiMesh* asssimp_mesh, std::string mesh_name, const aiMatrix4x4& mesh_transformation, uint32_t mesh_skeleton_uuid) const
{
	FileData mesh_data = App->resources->mesh_importer->ExtractMeshFromAssimp(asssimp_mesh, mesh_transformation, current_model_data.scale, mesh_skeleton_uuid);
	if (mesh_data.size == 0)
	{
		return 0;
	}

	uint32_t extracted_mesh_uuid = App->resources->CreateFromData(mesh_data, *current_model_data.asset_file_folder_path, mesh_name + ".mesh");
	return extracted_mesh_uuid;
}

uint32_t ModelImporter::ExtractSkeletonFromNode(const aiMesh* asssimp_mesh, std::string mesh_name) const
{
	uint32_t skeleton_uuid = 0;
	std::string main_bone_name = asssimp_mesh->mBones[0]->mName.C_Str();
	if (current_model_data.skeleton_cache.find(main_bone_name) != current_model_data.skeleton_cache.end())
	{
		skeleton_uuid = current_model_data.skeleton_cache[main_bone_name];
	}
	else
	{
		FileData skeleton_data = App->resources->skeleton_importer->ExtractSkeletonFromAssimp(current_model_data.scene, asssimp_mesh, current_model_data.scale);
		skeleton_uuid = App->resources->CreateFromData(skeleton_data, *current_model_data.asset_file_folder_path, mesh_name + "_skeleton.sk");

		if (skeleton_uuid != 0)
		{
			current_model_data.skeleton_cache[main_bone_name] = skeleton_uuid;
		}
	}

	return skeleton_uuid;
}

uint32_t ModelImporter::ExtractAnimationFromNode(const aiAnimation* assimp_animation, std::string animation_name) const
{
	FileData animation_data = App->resources->animation_importer->ExtractAnimationFromAssimp(current_model_data.scene, assimp_animation, current_model_data.scale);
	uint32_t extracted_animation_uuid = App->resources->CreateFromData(animation_data, *current_model_data.asset_file_folder_path, animation_name + ".anim");
	return extracted_animation_uuid;
}