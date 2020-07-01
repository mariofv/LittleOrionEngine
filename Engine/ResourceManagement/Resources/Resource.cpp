#include "Resource.h"

#include "Filesystem/Path.h"
#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/ResourcesDB/CoreResources.h"

Resource::Resource(uint32_t uuid) : uuid(uuid) {}

uint32_t Resource::GetUUID() const
{ 
	return uuid; 
}

bool Resource::IsCoreResource() const
{
	return uuid < NUM_CORE_RESOURCES;
}

std::string Resource::GetResourceTypeName(ResourceType resource_type)
{
	switch (resource_type)
	{
	case ResourceType::ANIMATION:
		return "Animation";
	case ResourceType::FONT:
		return "Font";
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
	case ResourceType::STATE_MACHINE:
		return "State Machine";
	case ResourceType::TEXTURE:
		return "Texture";
	case ResourceType::SOUND:
		return "SoundBank";
	default:
		return "Unknown";
	}
}