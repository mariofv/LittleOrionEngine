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

private:
	const std::string RESOURCE_DATABASE = "Resources/DB/resources.json";
	std::unordered_map<int64_t, std::unique_ptr<ImportOptions>> database;
};

#endif // !_RESOURCEDATABASE_H_



