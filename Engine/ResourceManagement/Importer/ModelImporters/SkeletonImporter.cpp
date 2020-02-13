#include "SkeletonImporter.h"
#include <assimp/scene.h>
#include <algorithm>

#include <Main/Application.h>
#include <Module/ModuleFileSystem.h>
bool SkeletonImporter::ImportSkeleton(const aiScene* scene, const aiMesh* mesh, const aiMatrix4x4& mesh_transformation, std::string& output_file) const
{

	aiString boneName = mesh->mBones[0]->mName;
	aiNode * bone = scene->mRootNode->FindNode(boneName);
	Skeleton skeleton("", "");
	Skeleton::Joint bone_joint{ GetTranform(bone->mTransformation), -1 };
	skeleton.skeleton.push_back(bone_joint);
	ImportBone(mesh, bone, skeleton.skeleton.size() - 1, bone->mTransformation, skeleton);

	if (skeleton.skeleton.size() > 0)
	{
		output_file = LIBRARY_SKELETON_FOLDER + "/" +  mesh->mName.C_Str()+ ".ol";
		//SaveBinary(skeleton, output_file);
	}
	return true;
}

void SkeletonImporter::ImportBone(const aiMesh* mesh, const aiNode * previus_node,  uint32_t previous_joint_index, const aiMatrix4x4& parent_transformation, Skeleton & skeleton) const
{
	aiMatrix4x4 current_transformation = parent_transformation * previus_node->mTransformation;
	for (size_t i = 0; i < previus_node->mNumChildren; i++)
	{
		aiString bone_name = previus_node->mChildren[i]->mName;
		aiBone * node_bone = GetNodeBone(mesh, bone_name);
		uint32_t next_joint = previous_joint_index;
		if (node_bone != nullptr) {
		
			Skeleton::Joint bone{ GetTranform(current_transformation), previous_joint_index };
			skeleton.skeleton.push_back(bone);
			next_joint = skeleton.skeleton.size() - 1;
		}
		ImportBone(mesh, previus_node->mChildren[i], next_joint, current_transformation, skeleton);
	}
}
aiBone* SkeletonImporter::GetNodeBone(const aiMesh* mesh,  const aiString & boneName) const
{
	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		if (mesh->mBones[i]->mName == boneName)
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

	uint32_t size =  sizeof(Skeleton::Joint) * num_bones;

	char* data = new char[size]; // Allocate
	char* cursor = data;
	size_t bytes = sizeof(uint32_t); // First store ranges
	memcpy(cursor, &num_bones, bytes);

	cursor += bytes; // Store bones
	bytes = sizeof(Skeleton::Joint) * num_bones;
	memcpy(cursor, &skeleton.skeleton.front(), bytes);

	App->filesystem->Save(output_file.c_str(), data, size);
	free(data);
	return true;
}