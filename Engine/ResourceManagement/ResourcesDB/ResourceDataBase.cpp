#include "ResourceDataBase.h"
#include "ResourceManagement/Importer/Importer.h"

void ResourceDataBase::AddEntry(const Path& meta_file)
{
	std::unique_ptr<ImportOptions> options = std::make_unique<ImportOptions>();
	Importer::GetOptionsFromMeta(meta_file,*options);
	if (entries.find(options->uuid) == entries.end())
	{
		entries[options->uuid] = std::move(options);
	}
}

void ResourceDataBase::AddEntry(const ImportOptions& importing_options)
{
	std::unique_ptr<ImportOptions> options = std::make_unique<ImportOptions>(importing_options);
	if (entries.find(options->uuid) == entries.end())
	{
		entries[options->uuid] = std::move(options);
	}
}

const ImportOptions* ResourceDataBase::GetEntry(uint32_t uuid)
{
	bool exist = entries.find(uuid) != entries.end();
	if (!exist)
	{
		return nullptr;
	}
	return entries[uuid].get();
}