#include "SkeletonImporter.h"
#include <assimp/scene.h>
#include <algorithm>

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
ImportResult SkeletonImporter::ImportSkeleton(const aiScene* scene, const aiMesh* mesh, const std::string& imported_file, float unit_scale_factor, Skeleton & skeleton) const
{

	skeleton.skeleton.resize(mesh->mNumBones);
	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		skeleton.skeleton[i].name = std::string(mesh->mBones[i]->mName.C_Str());
		skeleton.skeleton[i].transform_global = GetTransform(mesh->mBones[i]->mOffsetMatrix, unit_scale_factor);
	}

	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		aiString bone_name = mesh->mBones[i]->mName;
		aiNode * bone = scene->mRootNode->FindNode(bone_name);

		while (bone->mParent && bone->mParent != scene->mRootNode)
		{
			bone = bone->mParent;
			std::string node_name(bone->mName.C_Str());
			auto it = std::find_if(skeleton.skeleton.begin(), skeleton.skeleton.end(), [&node_name](const auto & joint) { return joint.name == node_name; });
			if (it != skeleton.skeleton.end())
			{
				skeleton.skeleton[i].parent_index = it - skeleton.skeleton.begin();
				break;
			}
		}
	}
	if (skeleton.skeleton.size() > 0)
	{
		skeleton.skeleton[0].parent_index = -1;
		std::string exported_file = SaveMetaFile(imported_file, ResourceType::SKELETON);
		skeleton.exported_file = exported_file;
		skeleton.imported_file = imported_file;
		SaveBinary(skeleton);
		return ImportResult{ true,exported_file };
		
	}
	return ImportResult();
}


float4x4 SkeletonImporter::GetTransform(const aiMatrix4x4& current_transform, float scale_factor) 
{
	aiVector3t<float> pScaling, pPosition;
	aiQuaterniont<float> pRotation;

	aiMatrix4x4 scale_matrix = aiMatrix4x4() * scale_factor ;
	scale_matrix[3][3] = 1;

	aiMatrix4x4 node_transformation = scale_matrix * current_transform * scale_matrix.Inverse();
	node_transformation.Decompose(pScaling, pRotation, pPosition);

	math::float3 scale(pScaling.x, pScaling.y,pScaling.z);
	math::Quat rotation(pRotation.x, pRotation.y, pRotation.z, pRotation.w);
	math::float3 translation(pPosition.x, pPosition.y, pPosition.z);
	return math::float4x4::FromTRS(translation, rotation, scale);
}

bool SkeletonImporter::SaveBinary(const Skeleton& skeleton) const
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
		memcpy(cursor, &joint.transform_global, sizeof(float4x4));
		cursor += sizeof(float4x4);
		memcpy(cursor, &joint.parent_index, sizeof(uint32_t));
		cursor += sizeof(uint32_t);
	}

	App->filesystem->Save(skeleton.exported_file.c_str(), data, size);
	App->filesystem->Save(skeleton.imported_file.c_str(), data, size);
	free(data);
	return true;
}