#include "ModelImporter.h"

#include "Helper/Config.h"

#include "Main/Application.h"
#include "MaterialImporter.h"
#include "ModelImporters/AnimationImporter.h"
#include "ModelImporters/MeshImporter.h"
#include "ModelImporters/SkeletonImporter.h"
#include "ModelImporters/ModelPrefabImporter.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/Resources/Mesh.h"
#include "TextureImporter.h"

#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/material.h"
#include "assimp/DefaultLogger.hpp"

#include "Brofiler/Brofiler.h"

ModelImporter::ModelImporter()
{
	Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Debugging), Assimp::Logger::Debugging);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Info), Assimp::Logger::Info);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Err), Assimp::Logger::Err);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Warn), Assimp::Logger::Warn);

	mesh_importer = std::make_unique<MeshImporter>();
	skeleton_importer = std::make_unique<SkeletonImporter>();
	animation_importer = std::make_unique<AnimationImporter>();
	model_prefab_importer = std::make_unique<ModelPrefabImporter>();
}

ModelImporter::~ModelImporter()
{
	Assimp::DefaultLogger::kill();
}

ImportResult ModelImporter::ImportExtractedResources(const File & file, bool force) const
{
	ImportResult result;
	if (file.file_type == FileType::MESH)
	{
		result = mesh_importer->Import(file, force);
	}
	return result;
}
ImportResult ModelImporter::Import(const File& file, bool force) const
{

	ImportResult import_result;
	
	if (file.filename.empty())
	{
		APP_LOG_ERROR("Importing mesh error: Couldn't find the file to import.")
		return import_result;
	}
	ImportOptions already_imported = GetAlreadyImportedResource(file);
	if (already_imported.uuid != 0 && !force) {
		import_result.success = true;
		import_result.exported_file = already_imported.exported_file;
		return import_result;
	}

	APP_LOG_INIT("Importing model %s.", file.file_path.c_str());

	performance_timer.Start();

	const aiScene* scene = aiImportFile(file.file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (scene == NULL )
	{
		const char *error = aiGetErrorString();
		APP_LOG_ERROR("Error loading model %s ", file.file_path.c_str());
		APP_LOG_ERROR(error);
		return import_result;
	}
	performance_timer.Stop();
	float time = performance_timer.Read();
	APP_LOG_SUCCESS("Model %s loaded correctly from assimp in %f ms.", file.file_path.c_str(), time);

	
	aiNode * root_node = scene->mRootNode;
	std::string base_path = file.file_path.substr(0, file.file_path.find_last_of("//"));
	aiMatrix4x4 identity_transformation = aiMatrix4x4();

	Config model;
	std::vector<Config> node_config = ImportNode(root_node, identity_transformation, scene, base_path.c_str());

	std::vector<Config> animations_config;
	for (size_t i = 0; i < scene->mNumAnimations; i++)
	{
		//Import animation
		Config animation_config;
		std::string library_animation_file;
		std::string assets_animation_file = base_path + "/"+file.filename_no_extension +"_"+scene->mAnimations[i]->mName.C_Str() + ".anim";
		animation_importer->ImportAnimation(scene, scene->mAnimations[i], assets_animation_file, library_animation_file);
		animation_config.AddString(library_animation_file, "Animation");
		animations_config.push_back(animation_config);
	}
	model.AddString(file.filename_no_extension, "Name");
	aiReleaseImport(scene);

	model.AddChildrenConfig(node_config, "Node");
	model.AddChildrenConfig(animations_config, "Animations");
	std::string output_file_model = SaveMetaFile(file.file_path, ResourceType::PREFAB);
	model_prefab_importer->ImportModelPrefab(model, output_file_model);


	import_result.success = true;
	import_result.exported_file = output_file_model;
	return import_result;
}

std::vector<Config> ModelImporter::ImportNode(const aiNode* root_node, const aiMatrix4x4& parent_transformation, const aiScene* scene, const std::string& base_path) const
{

	std::vector<Config> node_config;

	aiMatrix4x4& current_transformation = parent_transformation * root_node->mTransformation;
	// Transformation
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

	std::map<std::string, std::shared_ptr<Skeleton>> already_loaded_skeleton;
	for (size_t i = 0; i < root_node->mNumMeshes; ++i)
	{
		Config node;
		size_t mesh_index = root_node->mMeshes[i];


		std::string assets_material_file = base_path + "/" + std::string(root_node->mName.data) + std::to_string(i) + ".matol";
		std::string library_material_file = App->resources->material_importer->ExtractMaterialFromMesh(scene, mesh_index, base_path.c_str(), assets_material_file.c_str()).exported_file;
		node.AddString(library_material_file, "Material");

		std::shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>(0, "");
		aiMesh * importing_mesh = scene->mMeshes[mesh_index];
		if (importing_mesh->HasBones())
		{
			std::string main_bone_name = importing_mesh->mBones[0]->mName.C_Str();
			bool already_loaded = already_loaded_skeleton.find(main_bone_name) != already_loaded_skeleton.end();
			if (!already_loaded)
			{
				std::string assets_skeleton_file = base_path + "/" + std::string(root_node->mName.data) + "_skeleton" + std::to_string(i) + ".sk";
				ImportResult skeleton_import_result = skeleton_importer->ImportSkeleton(scene, importing_mesh, assets_skeleton_file, unit_scale_factor, *skeleton);
				if (skeleton_import_result.success)
				{
					already_loaded_skeleton[main_bone_name] = skeleton;
					node.AddString(skeleton_import_result.exported_file, "Skeleton");
				}
			}
			else if (already_loaded)
			{
				skeleton = already_loaded_skeleton[main_bone_name];
				node.AddString(skeleton->exported_file, "Skeleton");
			}
		}
		

		std::string assets_mesh_file = base_path + "/" + std::string(importing_mesh->mName.data) + std::to_string(i) + ".mesh";
		ImportResult mesh_import_result = mesh_importer->ImportMesh(importing_mesh, current_transformation, assets_mesh_file, unit_scale_factor, *skeleton);
		if (mesh_import_result.success)
		{
			node.AddString(mesh_import_result.exported_file, "Mesh");
			node.AddString(importing_mesh->mName.data, "Name");
		}
		node_config.push_back(node);
	}

	for (size_t i = 0; i < root_node->mNumChildren; i++)
	{
		std::vector<Config> child_node_config = ImportNode(root_node->mChildren[i], current_transformation, scene, base_path);
		node_config.insert(node_config.begin(), child_node_config.begin(), child_node_config.end());
	}

	return node_config;
}

