#ifndef _IMPORTER_H_
#define _IMPORTER_H_

#include "Main/Globals.h"
#include "ResourceManagement/ImportOptions/ImportOptions.h"

#include <algorithm>

class File;

class Importer {
public:
	Importer() = default;
	virtual ~Importer() = default;
	virtual std::pair<bool, std::string> Import(const File & file, bool force = false) const;

protected:

	ImportOptions GetAlreadyImportedResource(const File & file_to_look_for) const;
	void SaveMetaFile(const File & imported_file, ResourceType resource_type, const std::string & exported_path) const;

public:
	static void GetOptionsFromMeta(const File& file, ImportOptions & options);
	static std::string GetMetaFilePath(const File& file);
private:
	static const int IMPORTER_VERSION = 2;
};
#endif // !_IMPORTER_H_

