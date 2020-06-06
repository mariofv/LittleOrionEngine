#ifndef _METAFILE_H_
#define _METAFILE_H_

#include "ResourceManagement/Resources/Resource.h"
#include <string>

class Config;

class Metafile
{
public:
	Metafile() = default;
	virtual ~Metafile() = default;

	Metafile(const std::string& path) : metafile_path(path) {};

	//Copy and move
	Metafile(const Metafile& component_to_copy) = default;
	Metafile(Metafile&& component_to_move) = default;

	Metafile& operator=(const Metafile& component_to_copy) = default;
	Metafile& operator=(Metafile&& component_to_move) = default;

	virtual void Save(Config& config) const;
	virtual void Load(const Config& config);

public:
	uint32_t uuid = 0;
	std::string resource_name;

	ResourceType resource_type = ResourceType::UNKNOWN;

	std::string metafile_path = "";
	std::string imported_file_path = "";
	std::string exported_file_path = "";

	int version = 0;
};


#endif // _METAFILE_H_

