#ifndef _IMPORTER_H_
#define _IMPORTER_H_
#include "Main/Globals.h"
#include "Module/ModuleFileSystem.h"

#include <algorithm>
class File;
class Importer {
public:
	virtual std::pair<bool, std::string> Import(const File & file) const = 0;

protected:

	std::shared_ptr<File> GetAlreadyImportedResource(const std::string & path_to_look_in, const File & file_to_look_for) const;

};
#endif // !_IMPORTER_H_
