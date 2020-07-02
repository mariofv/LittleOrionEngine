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

FileData SkeletonImporter::ExtractSkeletonFromAssimp(const aiScene* scene, const aiMesh* mesh, float unit_scale_factor, bool complex_skeleton) const
{
	FileData skeleton_data;
	Skeleton imported_skeleton;

	imported_skeleton.skeleton.resize(mesh->mNumBones);
	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		imported_skeleton.skeleton[i].name = std::string(mesh->mBones[i]->mName.C_Str());
		imported_skeleton.skeleton[i].transform_global = SkeletonImporter::GetTransform(mesh->mBones[i]->mOffsetMatrix, unit_scale_factor);
		imported_skeleton.skeleton[i].parent_index = -1;
	}

	aiString bone_name = mesh->mBones[0]->mName;
	aiNode * bone = scene->mRootNode->FindNode(bone_name);

	//bone->mParent->mNumChildren <= 1 arbitrary rule just base in zombunny and player meshes
	while (bone->mParent && bone->mParent != scene->mRootNode)
	{
		bone = bone->mParent;
	}

	ImportRootBone(bone, bone->mTransformation, imported_skeleton, unit_scale_factor);
	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		aiString bone_name = mesh->mBones[i]->mName;
		aiNode * bone = scene->mRootNode->FindNode(bone_name);

		while (bone->mParent && bone->mParent != scene->mRootNode)
		{
			bone = bone->mParent;
			std::string node_name(bone->mName.C_Str());
			auto it = std::find_if(imported_skeleton.skeleton.begin(), imported_skeleton.skeleton.end(), [&node_name](const auto & joint) { return joint.name == node_name; });
			if (it != imported_skeleton.skeleton.end())
			{
				imported_skeleton.skeleton[i].parent_index = it - imported_skeleton.skeleton.begin();
				break;
			}
		}
	}
	if (imported_skeleton.skeleton.size() > 0)
	{
		skeleton_data = CreateBinary(imported_skeleton);
	}

	return skeleton_data;
}

void SkeletonImporter::ImportRootBone(const aiNode * previus_node, aiMatrix4x4& parent_global_transformation, Skeleton& skeleton, float unit_scale_factor) const
{
	for (size_t i = 0; i < previus_node->mNumChildren; i++)
	{
		aiNode* current_node = previus_node->mChildren[i];
		aiMatrix4x4 current_global_transformation = parent_global_transformation * current_node->mTransformation;
		std::string bone_name = std::string(current_node->mName.C_Str());

		if (bone_name.find("$Assimp") == std::string::npos)
		{
			auto it = std::find_if(skeleton.skeleton.begin(), skeleton.skeleton.end(), [&bone_name](const Skeleton::Joint & joint) { return joint.name == bone_name; });
			if (it == skeleton.skeleton.end())
			{
				Skeleton::Joint bone{ SkeletonImporter::GetTransform(current_global_transformation,unit_scale_factor),-1, bone_name };
				skeleton.skeleton.push_back(bone);
				return;
			}
		}
		ImportRootBone(current_node, current_global_transformation, skeleton, unit_scale_factor);
	}
}

float4x4 SkeletonImporter::GetTransform(const aiMatrix4x4& current_transform, float scale_factor) const
{
	aiVector3t<float> pScaling, pPosition;
	aiQuaterniont<float> pRotation;

	current_transform.Decompose(pScaling, pRotation, pPosition);
	pPosition *= scale_factor;


	math::float3 scale(1.f, 1.f, 1.f);
	math::Quat rotation(pRotation.x, pRotation.y, pRotation.z, pRotation.w);
	math::float3 translation(pPosition.x, pPosition.y, pPosition.z);
	return math::float4x4::FromTRS(translation, rotation, scale);
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
