#include "SkeletonImporter.h"
#include <assimp/scene.h>
#include <algorithm>

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
ImportResult SkeletonImporter::ImportSkeleton(const aiScene* scene, const aiMesh* mesh, const std::string& imported_file, float unit_scale_factor, Skeleton & skeleton) const
{
	skeleton.scale_factor = unit_scale_factor;

	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		aiString bone_name = mesh->mBones[i]->mName;
		aiNode * bone = scene->mRootNode->FindNode(bone_name);

		//bone->mParent->mNumChildren <= 1 arbitrary rule just base in zombunny and player meshes
		while (bone->mParent && bone->mParent != scene->mRootNode && bone->mParent->mNumChildren <= 1)
		{
			bone = bone->mParent;
		}

		ImportChildBone(bone, -1, bone->mTransformation, bone->mTransformation, skeleton);
	}

	if (skeleton.skeleton.size() > 0)
	{
		std::string exported_file = SaveMetaFile(imported_file, ResourceType::SKELETON);
		skeleton.exported_file = exported_file;
		skeleton.imported_file = imported_file;
		SaveBinary(skeleton);
		return ImportResult{ true,exported_file };
		
	}
	return ImportResult();
}

void SkeletonImporter::ImportChildBone(const aiNode * previus_node,  uint32_t previous_joint_index, aiMatrix4x4& parent_global_transformation,  aiMatrix4x4& accumulated_local_transformation, Skeleton& skeleton) const
{

	if (previous_joint_index == -1 && std::string(previus_node->mName.C_Str()).find("$Assimp") == std::string::npos) 
	{
		aiMatrix4x4 local_transformation = accumulated_local_transformation * previus_node->mTransformation;
		Skeleton::Joint bone{ GetTransform(local_transformation), GetTransform(local_transformation),previous_joint_index, std::string(previus_node->mName.C_Str()) };

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
		
			Skeleton::Joint bone{ GetTransform(current_global_transformation), GetTransform(local_transformation),previous_joint_index, bone_name};
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
		ImportChildBone(current_node, next_joint, current_global_transformation, local_transformation,skeleton);
	}
}

aiBone* SkeletonImporter::GetNodeBone(const aiMesh* mesh,  const std::string & boneName) const
{
	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		if (std::string(mesh->mBones[i]->mName.C_Str()) == boneName)
		{
			return mesh->mBones[i];
		}
	}
	return nullptr;
}


float4x4 SkeletonImporter::GetTransform(const aiMatrix4x4 & current_transform) 
{
	aiVector3t<float> pScaling, pPosition;
	aiQuaterniont<float> pRotation;
	aiMatrix4x4 node_transformation = current_transform;
	node_transformation.Decompose(pScaling, pRotation, pPosition);

	math::float3 scale(pScaling.x, pScaling.y,pScaling.z);
	math::Quat rotation(pRotation.x, pRotation.y, pRotation.z, pRotation.w);
	math::float3 translation(pPosition.x, pPosition.y, pPosition.z);

	return math::float4x4::FromTRS(translation, rotation, scale);
}

bool SkeletonImporter::SaveBinary(const Skeleton & skeleton) const
{

	uint32_t num_bones = skeleton.skeleton.size();

	uint32_t size = sizeof(float) + sizeof(uint32_t);

	for (auto & joint : skeleton.skeleton)
	{
		size += sizeof(uint32_t) * 2 + joint.name.size() + sizeof(float4x4)*2;
	}

	char* data = new char[size]; // Allocate
	char* cursor = data;

	memcpy(cursor, &skeleton.scale_factor, sizeof(float));
	cursor += sizeof(float);

	size_t bytes = sizeof(uint32_t); // First store ranges
	memcpy(cursor, &num_bones, bytes);
	cursor += bytes; // Store bones
	
	for (auto & joint : skeleton.skeleton)
	{

		uint32_t name_size = joint.name.size();
		memcpy(cursor, &name_size, sizeof(uint32_t));
		cursor +=sizeof(uint32_t);
		memcpy(cursor, joint.name.data(), name_size);
		cursor += name_size;
		memcpy(cursor, &joint.transform_global, sizeof(float4x4));
		cursor += sizeof(float4x4);
		memcpy(cursor, &joint.transform_local, sizeof(float4x4));
		cursor += sizeof(float4x4);
		memcpy(cursor, &joint.parent_index, sizeof(uint32_t));
		cursor += sizeof(uint32_t);
	}

	App->filesystem->Save(skeleton.exported_file.c_str(), data, size);
	App->filesystem->Save(skeleton.imported_file.c_str(), data, size);
	free(data);
	return true;
}