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
	void AddEntry(const Metafile& metafile);

	Metafile* GetEntry(uint32_t uuid);
	void GetEntriesOfType(std::vector<Metafile*> entries, ResourceType type) const;

public:
	std::unordered_map<int32_t, std::unique_ptr<Metafile>> entries;

private:
	const std::string RESOURCE_DATABASE = "Resources/DB/resources.json";
};

#endif // !_RESOURCEDATABASE_H_