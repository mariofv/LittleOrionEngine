#include "Resource.h"

#include "ResourceManagement/Metafile/Metafile.h"

Resource::Resource(Metafile* metafile) : resource_metafile(metafile) {}

uint32_t Resource::GetUUID() const
{ 
	return resource_metafile->uuid; 
}