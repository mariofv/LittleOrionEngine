#include "Resource.h"

#include "ResourceManagement/Metafile/Metafile.h"

Resource::Resource(Metafile* metafile) : resource_metafile(metafile) {}

uint32_t Resource::GetUUID() const
{ 
	return resource_metafile->uuid; 
}

std::string Resource::GetResourceTypeName(ResourceType resource_type)
{
	switch (resource_type)
	{
	case ResourceType::ANIMATION:
		return "Animation";
	case ResourceType::AUDIO:
		return "Audio";
	case ResourceType::MATERIAL:
		return "Material";
	case ResourceType::MODEL:
		return "Model";
	case ResourceType::MESH:
		return "Mesh";
	case ResourceType::NAVMESH:
		return "NavMesh";
	case ResourceType::PREFAB:
		return "Prefab";
	case ResourceType::SCENE:
		return "Scene";
	case ResourceType::SKELETON:
		return "Skeleton";
	case ResourceType::SKYBOX:
		return "Skybox";
	case ResourceType::TEXTURE:
		return "Texture";
	case ResourceType::UNKNOWN:
		return "Unknown";
	}
}