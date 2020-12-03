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
	Importer(ResourceType resource_type) : m_resource_type(resource_type) {};
	virtual ~Importer() = default;

	Metafile* GenericImport(Path& assets_file_path, ResourceType resourceType);
	Metafile* Import(Path& assets_file_path);
	virtual FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const;

	static bool ImportRequired(const Path& file_path);

public:
	ResourceType m_resource_type = ResourceType::UNKNOWN;
	static const int IMPORTER_VERSION = 12;
};
#endif // !_IMPORTER_H_

