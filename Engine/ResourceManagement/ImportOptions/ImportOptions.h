#ifndef _IMPORTOPTIONS_H_
#define _IMPORTOPTIONS_H_

#include "ResourceManagement/Resources/Resource.h"
#include <string>

class Config;

struct ImportOptions
{
	ImportOptions() = default;
	virtual ~ImportOptions() = default;

	//Copy and move
	ImportOptions(const ImportOptions& component_to_copy) = default;
	ImportOptions(ImportOptions&& component_to_move) = default;

	ImportOptions & operator=(const ImportOptions& component_to_copy) = default;
	ImportOptions & operator=(ImportOptions&& component_to_move) = default;

	virtual void Save(Config& config) const;
	virtual void Load(const Config& config);

	uint32_t uuid = 0;
	ResourceType resource_type = ResourceType::UNKNOWN;

	long long timestamp = 0; //PLACEHOLDER

	std::string exported_file;
	std::string imported_file;

	int version = 0;

};


#endif // !1

