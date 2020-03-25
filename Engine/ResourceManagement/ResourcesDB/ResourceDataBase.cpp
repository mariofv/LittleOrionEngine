#include "ResourceDataBase.h"
#include "ResourceManagement/Metafile/MetafileManager.h"

void ResourceDataBase::AddEntry(const Path& metafile_path)
{
	Metafile metafile;
	MetafileManager::GetMetafile(metafile_path, metafile);
	AddEntry(metafile);
}

void ResourceDataBase::AddEntry(const Metafile& metafile)
{
	std::unique_ptr<Metafile> options = std::make_unique<Metafile>(metafile);
	if (entries.find(options->uuid) == entries.end())
	{
		entries[options->uuid] = std::move(options);
	}
}

Metafile* ResourceDataBase::GetEntry(uint32_t uuid)
{
	bool exist = entries.find(uuid) != entries.end();
	if (!exist)
	{
		return nullptr;
	}
	return entries[uuid].get();
}

void ResourceDataBase::GetEntriesOfType(std::vector<Metafile*> entries, ResourceType type) const
{
	for (auto& entry : entries)
	{
		if (entry->resource_type == type)
		{
			entries.push_back(entry);
		}
	}
}
