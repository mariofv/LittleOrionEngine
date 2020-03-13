#include "ModelImporter.h"

#include "Helper/Config.h"

#include "Main/Application.h"
#include "MaterialImporter.h"
#include "ModelImporters/AnimationImporter.h"
#include "ModelImporters/MeshImporter.h"
#include "ModelImporters/SkeletonImporter.h"
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
	App->filesystem->MakeDirectory(LIBRARY_MATERIAL_FOLDER);
	App->filesystem->MakeDirectory(LIBRARY_MODEL_FOLDER);

	mesh_importer = std::make_unique<MeshImporter>();
	skeleton_importer = std::make_unique<SkeletonImporter>();
	animation_importer = std::make_unique<AnimationImporter>();
}

ModelImporter::~ModelImporter()
{
	Assimp::DefaultLogger::kill();
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
		import_result.succes = true;
		import_result.exported_file = already_imported.exported_file;
		return import_result;
	}

	File output_file = App->filesystem->MakeDirectory(LIBRARY_MESHES_FOLDER"/" + file.filename_no_extension);
	std::string output_file_model = LIBRARY_MODEL_FOLDER"/" + file.filename_no_extension + ".json";
	APP_LOG_INIT("Importing model %s.", file.file_path.c_str());

	performance_timer.Start();

	const aiScene* scene = aiImportFile(file.file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == NULL || output_file.file_path.empty())
	{
		const char *error = aiGetErrorString();
		APP_LOG_ERROR("Error loading model %s ", file.file_path.c_str());
		APP_LOG_ERROR(error);
		App->filesystem->Remove(&output_file);
		return import_result;
	}
	performance_timer.Stop();
	float time = performance_timer.Read();
	APP_LOG_SUCCESS("Model %s loaded correctly from assimp in %f ms.", file.file_path.c_str(), time);

	
	aiNode * root_node = scene->mRootNode;
	std::string base_path = file.file_path.substr(0, file.file_path.find_last_of("//"));
	aiMatrix4x4 identity_transformation = aiMatrix4x4();

	Config model;
	std::vector<Config> node_config = ImportNode(root_node, identity_transformation, scene, base_path.c_str(), output_file);

	std::vector<Config> animations_config;
	for (size_t i = 0; i < scene->mNumAnimations; i++)
	{
		Config animation_config;
		std::string animation;
		std::string animation_path = base_path + "/"+file.filename_no_extension +"_"+scene->mAnimations[i]->mName.C_Str() + ".anim";
		animation_importer->ImportAnimation(scene, scene->mAnimations[i], animation, animation_path);
		animation_config.AddString(animation, "Animation");
		animations_config.push_back(animation_config);
	}

	aiReleaseImport(scene);

	model.AddChildrenConfig(node_config, "Node");
	model.AddChildrenConfig(animations_config, "Animations");


	std::string serialized_model_string;
	model.GetSerializedString(serialized_model_string);
	App->filesystem->Save(output_file_model.c_str(), serialized_model_string.c_str(), serialized_model_string.size() + 1);

	SaveMetaFile(file.file_path, ResourceType::MESH, output_file_model);
	import_result.succes = true;
	import_result.exported_file = output_file_model;
	return import_result;
}

std::vector<Config> ModelImporter::ImportNode(const aiNode* root_node, const aiMatrix4x4& parent_transformation, const aiScene* scene, const std::string& base_path, const File& exported_file) const
{

	std::vector<Config> node_config;

	aiMatrix4x4& current_transformation = parent_transformation * root_node->mTransformation;

	for (size_t i = 0; i < root_node->mNumMeshes; ++i)
	{
		Config node;
		size_t mesh_index = root_node->mMeshes[i];


		std::string assets_material_file = base_path + "/" + std::string(root_node->mName.data) + std::to_string(i) + ".matol";
		std::string library_material_file = LIBRARY_MATERIAL_FOLDER"/" + std::string(root_node->mName.data) + std::to_string(i) + ".matol";
		App->resources->material_importer->ExtractMaterialFromMesh(scene, mesh_index, base_path.c_str(), assets_material_file.c_str());
		node.AddString(library_material_file, "Material");

		aiMesh * importing_mesh = scene->mMeshes[mesh_index];

		std::string assets_mesh_file = base_path + "/" + std::string(importing_mesh->mName.data) + std::to_string(i) + ".ol";
		std::string library_mesh_file = exported_file.file_path + "/" + std::string(root_node->mName.data) + std::to_string(i) + ".ol";
		bool imported = mesh_importer->ImportMesh(importing_mesh, current_transformation, library_mesh_file, assets_mesh_file);
		if (imported)
		{
			node.AddString(library_mesh_file, "Mesh");
		}

		if (importing_mesh->HasBones())
		{
			std::string library_skeleton_file;
			std::string assets_skeleton_file = base_path + "/" + std::string(root_node->mName.data)+ "_skeleton" + std::to_string(i) + ".sk";
			skeleton_importer->ImportSkeleton(scene, importing_mesh, library_skeleton_file, assets_skeleton_file);
			node.AddString(library_skeleton_file, "Skeleton");
		}
		node_config.push_back(node);
	}

	for (size_t i = 0; i < root_node->mNumChildren; i++)
	{
		std::vector<Config> child_node_config = ImportNode(root_node->mChildren[i], current_transformation, scene, base_path, exported_file);
		node_config.insert(node_config.begin(), child_node_config.begin(), child_node_config.end());
	}

	return node_config;
}

