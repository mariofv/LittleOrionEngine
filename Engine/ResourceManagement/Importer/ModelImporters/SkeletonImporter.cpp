#include "SkeletonImporter.h"

#include "Helper/Utils.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"

#include <assimp/scene.h>
#include <algorithm>

FileData SkeletonImporter::ExtractData(Path& assets_file_path, const Metafile& metafile) const
{
	return assets_file_path.GetFile()->Load();
}

FileData SkeletonImporter::ExtractSkeletonFromAssimp(const aiScene* scene, const aiMesh* mesh, float unit_scale_factor) const
{
	FileData skeleton_data;
	Skeleton imported_skeleton;

	//I now doing looooooooots of loops around here, but this is only importing and new to be 100% I have the hold hierarchy
	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		aiString bone_name = mesh->mBones[0]->mName;
		aiNode * bone = scene->mRootNode->FindNode(bone_name);

		while (bone->mParent && bone->mParent != scene->mRootNode)
		{
			bone = bone->mParent;
		}
		ImportChildBone(bone, -1, bone->mTransformation, bone->mTransformation, imported_skeleton, unit_scale_factor);
	}
	if (imported_skeleton.skeleton.size() > 0)
	{
		skeleton_data = CreateBinary(imported_skeleton);
	}

	return skeleton_data;
}

void SkeletonImporter::ImportChildBone(const aiNode * previus_node, uint32_t previous_joint_index, aiMatrix4x4& parent_global_transformation, aiMatrix4x4& accumulated_local_transformation, Skeleton& skeleton, float unit_scale_factor) const
{

	if (previous_joint_index == -1 && std::string(previus_node->mName.C_Str()).find("$Assimp") == std::string::npos)
	{
		aiMatrix4x4 local_transformation = accumulated_local_transformation * previus_node->mTransformation;
		Skeleton::Joint bone{ Utils::GetTransform(local_transformation,unit_scale_factor).Inverted(),previous_joint_index, std::string(previus_node->mName.C_Str()) };

		accumulated_local_transformation = aiMatrix4x4();
		auto it = std::find_if(skeleton.skeleton.begin(), skeleton.skeleton.end(), [&bone](const Skeleton::Joint & joint) { return joint.name == bone.name; });
		if (it == skeleton.skeleton.end())
		{
			skeleton.skeleton.push_back(bone);
		}
		previous_joint_index = 0;
		parent_global_transformation = local_transformation;
	}


	for (size_t i = 0; i < previus_node->mNumChildren; i++)
	{
		aiNode* current_node = previus_node->mChildren[i];
		aiMatrix4x4 node_transformation = current_node->mTransformation;
		aiMatrix4x4 current_global_transformation = parent_global_transformation * node_transformation;
		aiMatrix4x4 local_transformation = accumulated_local_transformation * node_transformation;
		std::string bone_name = std::string(current_node->mName.C_Str());

		uint32_t next_joint = previous_joint_index;
		if (bone_name.find("$Assimp") == std::string::npos)
		{

			Skeleton::Joint bone{ Utils::GetTransform(current_global_transformation,unit_scale_factor).Inverted(),previous_joint_index, bone_name };
			auto it = std::find_if(skeleton.skeleton.begin(), skeleton.skeleton.end(), [&bone_name](const Skeleton::Joint & joint) { return joint.name == bone_name; });
			if (it == skeleton.skeleton.end())
			{
				skeleton.skeleton.push_back(bone);
			}
			next_joint = skeleton.skeleton.size() - 1;
			if (next_joint == 0)
			{
				current_global_transformation = local_transformation;
			}
			local_transformation = aiMatrix4x4();
		}
		ImportChildBone(current_node, next_joint, current_global_transformation, local_transformation, skeleton, unit_scale_factor);
	}
}

FileData SkeletonImporter::CreateBinary(const Skeleton & skeleton) const
{
	uint32_t num_bones = skeleton.skeleton.size();

	uint32_t size = sizeof(uint32_t);

	for (const auto&  joint : skeleton.skeleton)
	{
		size += sizeof(uint32_t) * 2 + joint.name.size() + sizeof(float4x4);
	}

	char* data = new char[size]; // Allocate
	char* cursor = data;

	size_t bytes = sizeof(uint32_t); // First store ranges
	memcpy(cursor, &num_bones, bytes);
	cursor += bytes; // Store bones

	for (const auto& joint : skeleton.skeleton)
	{

		uint32_t name_size = joint.name.size();
		memcpy(cursor, &name_size, sizeof(uint32_t));
		cursor += sizeof(uint32_t);
		memcpy(cursor, joint.name.data(), name_size);
		cursor += name_size;
		memcpy(cursor, &joint.transform_global, sizeof(float4x4));
		cursor += sizeof(float4x4);
		memcpy(cursor, &joint.parent_index, sizeof(uint32_t));
		cursor += sizeof(uint32_t);
	}

	return FileData{ data, size };
}