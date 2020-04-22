#include "Skeleton.h"

#include "ResourceManagement/Metafile/Metafile.h"

Skeleton::Skeleton(uint32_t uuid) : Resource(uuid) {};

Skeleton::Skeleton(uint32_t uuid, std::vector<Joint> && joints) : skeleton(joints), Resource(uuid) {}