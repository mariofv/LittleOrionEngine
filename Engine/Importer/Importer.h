#ifndef _IMPORTER_H_
#define _IMPORTER_H_
#include "Globals.h"
#include "Module/ModuleFileSystem.h"

#include <algorithm>

class Importer {
public:
	virtual bool Import(const std::string& file, std::string& output_file) const = 0;

protected:

	std::shared_ptr<ModuleFileSystem::File> GetAlreadyImportedResource(const std::string & path_to_look_in, const ModuleFileSystem::File & file_to_look_for) const;

};
#endif // !_IMPORTER_H_

