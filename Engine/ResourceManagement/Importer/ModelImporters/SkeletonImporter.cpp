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

	imported_skeleton.skeleton.resize(mesh->mNumBones);
	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		imported_skeleton.skeleton[i].name = std::string(mesh->mBones[i]->mName.C_Str());
		imported_skeleton.skeleton[i].transform_global = Utils::GetTransform(mesh->mBones[i]->mOffsetMatrix, unit_scale_factor);
		imported_skeleton.skeleton[i].parent_index = -1;
	}

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