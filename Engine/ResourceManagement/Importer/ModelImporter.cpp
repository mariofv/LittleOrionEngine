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
#include "ResourceManagement/Metafile/ModelMetafile.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/DefaultLogger.hpp>

#include <Brofiler/Brofiler.h>

#include <fstream>


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
	ModelMetafile& model_metafile = static_cast<ModelMetafile&>(const_cast<Metafile&>(metafile));
	FileData model_data{NULL, 0};

	// LOAD ASSIMP SCENE
	RESOURCES_LOG_INFO("Importing model %s.", assets_file_path.GetFullPath().c_str())
	performance_timer.Start();
	FileData file_data = assets_file_path.GetFile()->Load();
	const aiScene* scene = aiImportFileFromMemory((char*)file_data.buffer, file_data.size, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	if (scene == NULL)
	{
		const char *error = aiGetErrorString();
		RESOURCES_LOG_ERROR("Error loading model %s ", assets_file_path.GetFullPath().c_str());
		RESOURCES_LOG_ERROR(error);
		return model_data;
	}
	performance_timer.Stop();
	float time = performance_timer.Read();
	RESOURCES_LOG_INFO("Model %s loaded correctly from assimp in %f ms.", assets_file_path.GetFullPath().c_str(), time);


	// LOCATE PATH OF MODEL FILE
	Path* asset_file_folder_path = assets_file_path.GetParent();


	// LOAD NATIVE SCALE
	float unit_scale_factor = 1.f;
	if (model_metafile.convert_units)
	{
		for (unsigned int i = 0; i < scene->mMetaData->mNumProperties; ++i)
		{
			if (scene->mMetaData->mKeys[i] == aiString("UnitScaleFactor"))
			{
				aiMetadataEntry unit_scale_entry = scene->mMetaData->mValues[i];
				unit_scale_factor = *(double*)unit_scale_entry.mData;
			};
		}
		unit_scale_factor *= model_metafile.scale_factor;
	}


	// INIT SKELETON CACHE
	std::map<std::string, uint32_t> skeleton_cache;


	// STORE ALL MODEL GLOBAL DATA
	current_model_data = {scene, asset_file_folder_path, unit_scale_factor, skeleton_cache};
	current_model_data.model_metafile = &model_metafile;
	current_model_data.animated_model = false;
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		current_model_data.animated_model |= scene->mMeshes[i]->HasBones();
	}

	aiNode* root_node = scene->mRootNode;
	aiMatrix4x4 identity_transformation = aiMatrix4x4();
	std::vector<Config> node_config = ExtractDataFromNode(root_node, identity_transformation);
	Config model;
	model.AddString(assets_file_path.GetFilenameWithoutExtension(), "Name");

	if (model_metafile.import_animation) {
		std::vector<Config> animations_config;
		for (size_t i = 0; i < scene->mNumAnimations; i++)
		{
			//Import animation
			Config animation_config;
			std::string animation_name = assets_file_path.GetFilenameWithoutExtension() + "_" + scene->mAnimations[i]->mName.C_Str();
			uint32_t extracted_animation_uuid = ExtractAnimationFromNode(scene->mAnimations[i], animation_name);

			animation_config.AddUInt(extracted_animation_uuid, "Animation");
			animations_config.push_back(animation_config);
		}

		model.AddChildrenConfig(node_config, "Node");
		model.AddChildrenConfig(animations_config, "Animations");
	}
	aiReleaseImport(scene);

	model_data = App->resources->prefab_importer->ExtractFromModel(model, model_metafile, current_model_data.animated_model);
	if (current_model_data.remmaped_changed || current_model_data.any_new_node)
	{
		App->resources->metafile_manager->SaveMetafile(static_cast<Metafile*>(&model_metafile), assets_file_path);
	}

	return model_data;
}

std::vector<Config> ModelImporter::ExtractDataFromNode(const aiNode* root_node, const aiMatrix4x4& parent_transformation) const
{
	std::vector<Config> node_config;

	aiMatrix4x4& current_transformation = parent_transformation * root_node->mTransformation;
	// Transformation
	aiVector3D model_position;
	aiVector3D model_scale;
	aiQuaternion model_rotation;

	current_transformation.Decompose(model_scale, model_rotation, model_position);

	float3 translation = float3(model_position.x, model_position.y, model_position.z);
	Quat rotation = Quat(model_rotation.x, model_rotation.y, model_rotation.z, model_rotation.w);
	float3 scale = float3(model_scale.x, model_scale.y, model_scale.z);

	std::map<std::string, std::shared_ptr<Skeleton>> already_loaded_skeleton;
	for (size_t i = 0; i < root_node->mNumMeshes; ++i)
	{
		Config node;
		size_t mesh_index = root_node->mMeshes[i];
		aiMesh* node_mesh = current_model_data.scene->mMeshes[mesh_index];
		std::string mesh_name = std::string(root_node->mName.data);
		node.AddString(mesh_name, "Name");
		node.AddFloat3(translation, "Translation");
		node.AddQuat(rotation, "Rotation");
		node.AddFloat3(scale, "Scale");

		if (current_model_data.model_metafile->import_material)
		{
			uint32_t extracted_material_uuid = ExtractMaterialFromNode(mesh_index, mesh_name);
			auto & remapped_materials = current_model_data.model_metafile->remapped_materials;
			current_model_data.remmaped_changed = remapped_materials[mesh_name] == extracted_material_uuid;
			if (remapped_materials.find(mesh_name) == remapped_materials.end() || current_model_data.remmaped_changed)
			{
				remapped_materials[mesh_name] = 0;
			}
			node.AddUInt(extracted_material_uuid, "Material");
		}
		
		uint32_t extracted_skeleton_uuid = 0;
		if (node_mesh->HasBones() && current_model_data.model_metafile->import_rig)
		{
			extracted_skeleton_uuid = ExtractSkeletonFromNode(node_mesh, mesh_name);
			if (extracted_skeleton_uuid != 0)
			{
				node.AddUInt(extracted_skeleton_uuid, "Skeleton");
			}
		}

		if (current_model_data.model_metafile->import_mesh)
		{
			uint32_t extracted_mesh_uuid = ExtractMeshFromNode(node_mesh, mesh_name, parent_transformation, extracted_skeleton_uuid);
			if (extracted_mesh_uuid != 0)
			{
				node.AddUInt(extracted_mesh_uuid, "Mesh");
			}
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

	Metafile node;
	node.resource_type = ResourceType::MATERIAL;
	node.resource_name = mesh_name + ".mat";
	return SaveDataInLibrary( node, mesh_material_data);
}

uint32_t ModelImporter::SaveDataInLibrary( Metafile &node_metafile, FileData & file_data) const
{
	node_metafile.metafile_path = current_model_data.model_metafile->metafile_path;
	node_metafile.imported_file_path = current_model_data.model_metafile->imported_file_path;
	current_model_data.model_metafile->GetModelNode(node_metafile);
	bool is_new_node = node_metafile.uuid == 0;
	node_metafile.uuid = is_new_node ? pcg32_random() : node_metafile.uuid;
	node_metafile.exported_file_path = App->resources->metafile_manager->GetUUIDExportedFolder(node_metafile.uuid);
	if (!App->filesystem->Exists(node_metafile.exported_file_path))
	{
		App->filesystem->MakeDirectory(node_metafile.exported_file_path);
	}
	Path* metafile_exported_folder_path = App->filesystem->GetPath(node_metafile.exported_file_path);
	
	node_metafile.exported_file_path = metafile_exported_folder_path->Save(std::to_string(node_metafile.uuid).c_str(), file_data)->GetFullPath();
	if (is_new_node)
	{
		current_model_data.any_new_node = true;
		current_model_data.model_metafile->nodes.push_back(std::make_unique<Metafile>(node_metafile));
	}
	return node_metafile.uuid;
}

uint32_t ModelImporter::ExtractMeshFromNode(const aiMesh* asssimp_mesh, std::string mesh_name, const aiMatrix4x4& mesh_transformation, uint32_t mesh_skeleton_uuid) const
{
	FileData mesh_data = App->resources->mesh_importer->ExtractMeshFromAssimp(asssimp_mesh, mesh_transformation, current_model_data.scale, mesh_skeleton_uuid, current_model_data.animated_model);
	if (mesh_data.size == 0)
	{
		return 0;
	}

	Metafile node;
	node.resource_type = ResourceType::MESH;
	node.resource_name = mesh_name + ".mesh";
	return SaveDataInLibrary(node, mesh_data);
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
		FileData skeleton_data = App->resources->skeleton_importer->ExtractSkeletonFromAssimp(current_model_data.scene, asssimp_mesh, current_model_data.scale, current_model_data.model_metafile->complex_skeleton);

		Metafile node;
		node.resource_type = ResourceType::SKELETON;
		node.resource_name = mesh_name + ".sk";
		skeleton_uuid =  SaveDataInLibrary(node, skeleton_data);
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

	Metafile node;
	node.resource_type = ResourceType::ANIMATION;
	node.resource_name = animation_name + ".anim";
	return SaveDataInLibrary(node, animation_data);
}