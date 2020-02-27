#include "ModelImporter.h"
#include "ResourceManagement/Resources/Mesh.h"
#include "TextureImporter.h"
#include "Main/Application.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include "assimp/DefaultLogger.hpp"
#include "Brofiler/Brofiler.h"
#include "Helper/Config.h"


#include "ModelImporters/MeshImporter.h"
#include "ModelImporters/MaterialImporter.h"
#include "ModelImporters/SkeletonImporter.h"
#include "ModelImporters/AnimationImporter.h"

ModelImporter::ModelImporter()
{
	Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Debugging), Assimp::Logger::Debugging);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Info), Assimp::Logger::Info);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Err), Assimp::Logger::Err);
	Assimp::DefaultLogger::get()->attachStream(new AssimpStream(Assimp::Logger::Warn), Assimp::Logger::Warn);
	App->filesystem->MakeDirectory(LIBRARY_MODEL_FOLDER);

	mesh_importer = std::make_unique<MeshImporter>();
	material_importer = std::make_unique<MaterialImporter>();
	skeleton_importer = std::make_unique<SkeletonImporter>();
	animation_importer = std::make_unique<AnimationImporter>();

}

ModelImporter::~ModelImporter()
{
	Assimp::DefaultLogger::kill();
}

std::pair<bool, std::string> ModelImporter::Import(const File & file) const
{
	if (file.filename.empty())
	{
		APP_LOG_ERROR("Importing mesh error: Couldn't find the file to import.")
		return std::pair<bool, std::string>(false, "");
	}
	std::string already_imported = GetAlreadyImportedResource(file);
	if (!already_imported.empty()) {
		return std::pair<bool, std::string>(true, already_imported);
	}

	File output_file = App->filesystem->MakeDirectory(LIBRARY_MESHES_FOLDER+"/"+ file.filename_no_extension);
	std::string output_file_model = LIBRARY_MODEL_FOLDER + "/" + file.filename_no_extension + ".json";
	APP_LOG_INIT("Importing model %s.", file.file_path.c_str());

	performance_timer.Start();

	const aiScene* scene = aiImportFile(file.file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == NULL || output_file.file_path.empty())
	{
		const char *error = aiGetErrorString();
		APP_LOG_ERROR("Error loading model %s ", file.file_path.c_str());
		APP_LOG_ERROR(error);
		App->filesystem->Remove(&output_file);
		return std::pair<bool, std::string>(false, "");
	}
	performance_timer.Stop();
	float time = performance_timer.Read();
	APP_LOG_SUCCESS("Model %s loaded correctly from assimp in %f ms.", file.file_path.c_str(), time);

	
	aiNode * root_node = scene->mRootNode;
	std::string base_path = file.file_path.substr(0, file.file_path.find_last_of("//"));
	aiMatrix4x4 identity_transformation = aiMatrix4x4();

	Config model;
	std::vector<Config> node_config;
	ImportNode(root_node, identity_transformation, scene, base_path.c_str(),output_file.file_path, node_config);

	std::vector<Config> animations_config;
	for (size_t i = 0; i < scene->mNumAnimations; i++)
	{
		Config animation_config;
		std::string animation;
		animation_importer->ImportAnimation(scene, scene->mAnimations[i], animation);
		animation_config.AddString(animation, "Animation");
		animations_config.push_back(animation_config);
	}

	aiReleaseImport(scene);

	model.AddChildrenConfig(node_config, "Node");
	model.AddChildrenConfig(animations_config, "Animations");


	std::string serialized_model_string;
	model.GetSerializedString(serialized_model_string);
	App->filesystem->Save(output_file_model.c_str(), serialized_model_string.c_str(), serialized_model_string.size() + 1);

	SaveMetaFile(file, output_file_model);
	return std::pair<bool, std::string>(true, output_file.file_path);
}

void ModelImporter::ImportNode(const aiNode* root_node, const aiMatrix4x4& parent_transformation, const aiScene* scene, const char* file_path, const std::string& output_file,  std::vector<Config> & node_config) const
{

	aiMatrix4x4& current_transformation = parent_transformation * root_node->mTransformation;

	for (size_t i = 0; i < root_node->mNumMeshes; ++i)
	{
		Config node;
		size_t mesh_index = root_node->mMeshes[i];
		std::vector<Config> materials;
		std::vector<std::string> loaded_meshes_materials;
		material_importer->ImportMaterialFromMesh(scene, mesh_index, file_path, loaded_meshes_materials);


		for (size_t i = 0; i < loaded_meshes_materials.size(); i++)
		{
			Config texture;
			texture.AddString(loaded_meshes_materials[i], "uid");
			materials.push_back(texture);
		}
		node.AddChildrenConfig(materials, "Textures");

		std::string mesh_file = output_file + "/" + std::string(root_node->mName.data) + std::to_string(i) + ".ol";

		// Transformation
		aiVector3t<float> pScaling, pPosition;
		aiQuaterniont<float> pRotation;
		aiMatrix4x4 node_transformation = current_transformation;
		node_transformation.Decompose(pScaling, pRotation, pPosition);
		pScaling *= SCALE_FACTOR;
		pPosition *= SCALE_FACTOR;

		node_transformation = aiMatrix4x4(pScaling, pRotation, pPosition);
		aiMesh * importing_mesh = scene->mMeshes[mesh_index];
		bool imported = mesh_importer->ImportMesh(importing_mesh, node_transformation, mesh_file);
		if (imported)
		{
			node.AddString(mesh_file, "Mesh");
		}

		if (importing_mesh->HasBones())
		{
			std::string skeleton_file; 
			skeleton_importer->ImportSkeleton(scene, importing_mesh, skeleton_file);
			node.AddString(skeleton_file, "Skeleton");
		}
		node_config.push_back(node);
	}

	for (size_t i = 0; i < root_node->mNumChildren; i++)
	{
		ImportNode(root_node->mChildren[i], current_transformation, scene, file_path,output_file, node_config);
	}
}

