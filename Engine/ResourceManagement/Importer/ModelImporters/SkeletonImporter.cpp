#include "SkeletonImporter.h"
#include <assimp/scene.h>
#include <algorithm>

#include <Main/Application.h>
#include <Module/ModuleFileSystem.h>
bool SkeletonImporter::ImportSkeleton(const aiScene* scene, const aiMesh* mesh, const aiMatrix4x4& mesh_transformation, std::string& output_file) const
{

	aiString bone_name = mesh->mBones[0]->mName;
	aiNode * bone = scene->mRootNode->FindNode(bone_name);
	while (bone->mParent && bone->mParent != scene->mRootNode)
	{
		bone = bone->mParent;
	}

	Skeleton skeleton("", "");

	ImportChildBone(mesh, bone, - 1, bone->mTransformation, skeleton);

	if (skeleton.skeleton.size() > 0)
	{
		App->filesystem->MakeDirectory(LIBRARY_SKELETON_FOLDER);
		output_file = LIBRARY_SKELETON_FOLDER + "/" +  mesh->mName.C_Str()+ ".ol";
		SaveBinary(skeleton, output_file);
	}
	return true;
}

void SkeletonImporter::ImportChildBone(const aiMesh* mesh, const aiNode * previus_node,  uint32_t previous_joint_index, const aiMatrix4x4& parent_transformation, Skeleton & skeleton) const
{

	if (previous_joint_index == -1 && std::string(previus_node->mName.C_Str()).find("$Assimp") == std::string::npos) {

		Skeleton::Joint bone{ GetTranform(previus_node->mTransformation), previous_joint_index, std::string(previus_node->mName.C_Str()) };
		skeleton.skeleton.push_back(bone);
		previous_joint_index = 0;
	}
	aiMatrix4x4 current_transformation = parent_transformation * previus_node->mTransformation;

	for (size_t i = 0; i < previus_node->mNumChildren; i++)
	{
		std::string bone_name = std::string(previus_node->mChildren[i]->mName.C_Str());
		aiBone * node_bone = GetNodeBone(mesh, bone_name);
		uint32_t next_joint = previous_joint_index;
		if (node_bone != nullptr || bone_name.find("$Assimp") == std::string::npos) {
		
			Skeleton::Joint bone{ GetTranform(current_transformation), previous_joint_index, bone_name};
			skeleton.skeleton.push_back(bone);
			next_joint = skeleton.skeleton.size() - 1;
		}
		ImportChildBone(mesh, previus_node->mChildren[i], next_joint, current_transformation, skeleton);
	}
}

void SkeletonImporter::ImportParentBone(const aiMesh* mesh, const aiNode * previus_node, uint32_t previous_joint_index, const aiMatrix4x4& parent_transformation, Skeleton & skeleton) const
{
	aiMatrix4x4 current_transformation = parent_transformation * previus_node->mTransformation;
	for (size_t i = 0; i < previus_node->mNumChildren; i++)
	{
		std::string bone_name = std::string(previus_node->mChildren[i]->mName.C_Str());
		aiBone * node_bone = GetNodeBone(mesh, bone_name);
		uint32_t next_joint = previous_joint_index;
		if (node_bone != nullptr || bone_name.find("$Assimp") == std::string::npos) {

			Skeleton::Joint bone{ GetTranform(current_transformation), previous_joint_index, bone_name };
			skeleton.skeleton.push_back(bone);
			next_joint = skeleton.skeleton.size() - 1;
		}
		ImportChildBone(mesh, previus_node->mChildren[i], next_joint, current_transformation, skeleton);
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


float4x4 SkeletonImporter::GetTranform(const aiMatrix4x4 & current_transform) const
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

bool SkeletonImporter::SaveBinary(const Skeleton & skeleton, const std::string& output_file) const
{

	uint32_t num_bones = skeleton.skeleton.size();

	uint32_t size = sizeof(uint32_t);

	for (auto & joint : skeleton.skeleton)
	{
		size += sizeof(uint32_t) * 2 + joint.name.size() + sizeof(float4x4);
	}

	char* data = new char[size]; // Allocate
	char* cursor = data;
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
		memcpy(cursor, &joint.transform, sizeof(float4x4));
		cursor += sizeof(float4x4);
		memcpy(cursor, &joint.parent_index, sizeof(uint32_t));
		cursor += sizeof(uint32_t);
	}

	App->filesystem->Save(output_file.c_str(), data, size);
	free(data);
	return true;
}