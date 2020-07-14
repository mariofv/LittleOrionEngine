#ifndef _IMPORTER_H_
#define _IMPORTER_H_

#include "Filesystem/File.h"
#include "Main/Globals.h"
#include "ResourceManagement/Resources/Resource.h"

#include <algorithm>

class Path;
class Metafile;

class Importer 
{
public:
	Importer() = default;
	Importer(ResourceType resource_type) : resource_type(resource_type) {};
	virtual ~Importer() = default;

	Metafile* Import(Path& assets_file_path);
	virtual FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const = 0;

	static bool ImportRequired(const Path& file_path);

public:
	ResourceType resource_type = ResourceType::UNKNOWN;
	static const int IMPORTER_VERSION = 10;
};
#endif // !_IMPORTER_H_

