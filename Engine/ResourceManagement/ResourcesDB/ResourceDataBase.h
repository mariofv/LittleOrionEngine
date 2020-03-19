#ifndef _RESOURCEDATABASE_H_
#define _RESOURCEDATABASE_H_

#include "ResourceManagement/ImportOptions/ImportOptions.h"

#include <string>
#include <unordered_map>

class File;
class ResourceDataBase
{

public:
	ResourceDataBase() = default;
	~ResourceDataBase() = default;

	void AddEntry(const File& meta_file);
	void AddEntry(const ImportOptions& meta_file);

	const ImportOptions* GetEntry(uint32_t uuid);

public:
	std::unordered_map<int32_t, std::unique_ptr<ImportOptions>> entries;

private:
	const std::string RESOURCE_DATABASE = "Resources/DB/resources.json";
};

#endif // !_RESOURCEDATABASE_H_



