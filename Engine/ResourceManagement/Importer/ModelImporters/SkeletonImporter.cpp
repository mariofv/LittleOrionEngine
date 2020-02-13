#include "SkeletonImporter.h"
#include <assimp/scene.h>
#include <algorithm>

#include <Main/Application.h>
#include <Module/ModuleFileSystem.h>
bool SkeletonImporter::ImportSkeleton(const aiScene* scene, const aiMesh* mesh, const aiMatrix4x4& mesh_transformation, const std::string& output_file) const
{

	aiString boneName = mesh->mBones[0]->mName;
	aiNode * bone = scene->mRootNode->FindNode(boneName);
	Skeleton skeleton("", "");
	Skeleton::Joint bone_joint{ GetTranform(bone->mTransformation), nullptr };
	skeleton.skeleton.push_back(bone_joint);
	ImportBone(mesh, bone, skeleton.skeleton.back(), bone->mTransformation, skeleton);
	SaveBinary(skeleton, output_file);
	return true;
}

void SkeletonImporter::ImportBone(const aiMesh* mesh, const aiNode * previus_node,  Skeleton::Joint & previous_joint, const aiMatrix4x4& parent_transformation, Skeleton & skeleton) const
{
	aiMatrix4x4 current_transformation = parent_transformation * previus_node->mTransformation;
	for (size_t i = 0; i < previus_node->mNumChildren; i++)
	{
		aiString bone_name = previus_node->mChildren[i]->mName;
		aiBone * node_bone = GetNodeBone(mesh, bone_name);
		Skeleton::Joint * next_joint = &previous_joint;
		if (node_bone != nullptr) {
		
			Skeleton::Joint bone{ GetTranform(current_transformation), &previous_joint};
			skeleton.skeleton.push_back(bone);
			next_joint = &skeleton.skeleton.back();
		}
		ImportBone(mesh, previus_node->mChildren[i], *next_joint, current_transformation, skeleton);
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

	struct B_Bone {
		float4x4 transform;
		uint32_t index; 
	};
	std::vector<B_Bone> bones;

	for (auto & sk_bone : skeleton.skeleton)
	{

		auto it = std::find_if(skeleton.skeleton.begin(), skeleton.skeleton.end(), [&sk_bone](const Skeleton::Joint & joint) {
			&sk_bone == joint.parent;
		});
		uint32_t parent_index = -1;
		if (it != skeleton.skeleton.end())
		{
			parent_index = it - skeleton.skeleton.begin();
		}
		bones.push_back({sk_bone.transform, parent_index });

	}
	uint32_t num_bones = skeleton.skeleton.size();

	uint32_t size =  sizeof(B_Bone) * num_bones;

	char* data = new char[size]; // Allocate
	char* cursor = data;
	size_t bytes = sizeof(uint32_t); // First store ranges
	memcpy(cursor, &num_bones, bytes);

	cursor += bytes; // Store bones
	bytes = sizeof(B_Bone) * num_bones;
	memcpy(cursor, &bones.front(), bytes);

	App->filesystem->Save(output_file.c_str(), data, size);
	delete data;
	return true;
}