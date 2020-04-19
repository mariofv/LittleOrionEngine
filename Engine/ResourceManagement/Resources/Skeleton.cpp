#include "Skeleton.h"

#include "ResourceManagement/Metafile/Metafile.h"

Skeleton::Skeleton(Metafile* resource_metafile) : Resource(resource_metafile) {};

Skeleton::Skeleton(Metafile* resource_metafile, std::vector<Joint> && joints) : skeleton(joints), Resource(resource_metafile) {}