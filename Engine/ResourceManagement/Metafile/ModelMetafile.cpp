#include "ModelMetafile.h"

#include "Helper/Config.h"

void ModelMetafile::Save(Config& config) const
{
	Metafile::Save(config);
}

void ModelMetafile::Load(const Config& config)
{
	Metafile::Load(config);
}
