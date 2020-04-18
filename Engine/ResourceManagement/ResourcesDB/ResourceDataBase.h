#ifndef _RESOURCEDATABASE_H_
#define _RESOURCEDATABASE_H_

#include "ResourceManagement/Metafile/Metafile.h"

#include <string>
#include <unordered_map>

class Path;

class ResourceDataBase
{

public:
	ResourceDataBase() = default;
	~ResourceDataBase() = default;

	void AddEntry(const Path& metafile_path);
	void AddEntry(Metafile* metafile);

	Metafile* GetEntry(uint32_t uuid);
	void GetEntriesOfType(std::vector<Metafile*>& result_entries, ResourceType type) const;

public:
	std::unordered_map<uint32_t, Metafile*> entries;
};

#endif // !_RESOURCEDATABASE_H_