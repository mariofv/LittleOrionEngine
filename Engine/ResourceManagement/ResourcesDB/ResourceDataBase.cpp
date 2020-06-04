#include "ResourceDataBase.h"

#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Metafile/MetafileManager.h"
#include "ResourceManagement/Metafile/ModelMetafile.h"

void ResourceDataBase::AddEntry(const Path& metafile_path)
{
	Metafile* metafile = App->resources->metafile_manager->GetMetafile(metafile_path);
	AddEntry(metafile);
}

void ResourceDataBase::AddEntry(Metafile* metafile)
{
	if (entries.find(metafile->uuid) == entries.end())
	{
		entries[metafile->uuid] = metafile;
		if (metafile->resource_type == ResourceType::MODEL)
		{
			for (auto & node : static_cast<ModelMetafile*>(metafile)->nodes)
			{
				entries[node->uuid] = node.get();
			}
		}
	}
}

Metafile* ResourceDataBase::GetEntry(uint32_t uuid)
{
	bool exist = entries.find(uuid) != entries.end();
	if (!exist)
	{
		return nullptr;
	}
	return entries[uuid];
}

void ResourceDataBase::GetEntriesOfType(std::vector<Metafile*>& result_entries, ResourceType type) const
{
	for (auto& entry : entries)
	{
		if (entry.second->resource_type == type)
		{
			result_entries.push_back(entry.second);
		}
	}
}
