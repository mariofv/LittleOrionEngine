#ifndef _SKELETONIMPORTER_H_
#define _SKELETONIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/Skeleton.h"

#include <assimp/mesh.h>
#include <string>

struct aiScene;
struct aiNode;

class SkeletonImporter : public Importer
{
public:
	SkeletonImporter() : Importer(ResourceType::SKELETON) {};
	~SkeletonImporter() = default;

	FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const override;
	FileData ExtractSkeletonFromAssimp(const aiScene* scene, const aiMesh* mesh, float unit_scale_factor, bool complex_skeleton) const;

private:
	FileData CreateBinary(const Skeleton & skeleton) const;
	void ImportRootBone(const aiNode * previus_node, aiMatrix4x4& parent_global_transformation, Skeleton& skeleton, float unit_scale_factor) const;
	float4x4 GetTransform(const aiMatrix4x4& current_transform, float scale_factor = 1.0) const;
};

#endif