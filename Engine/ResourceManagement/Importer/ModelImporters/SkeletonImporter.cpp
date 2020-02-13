#include "SkeletonImporter.h"
#include <assimp/scene.h>

bool SkeletonImporter::ImportSkeleton(const aiScene* scene, const aiMesh* mesh, const aiMatrix4x4& mesh_transformation, const std::string& output_file) const
{
	for (size_t i = 0; i < mesh->mNumBones; i++)
	{
		aiString boneName = mesh->mBones[i]->mName;
		aiNode * bone = scene->mRootNode->FindNode(boneName);
		int n = bone->mNumChildren;
	
		
	}
	return true;
}