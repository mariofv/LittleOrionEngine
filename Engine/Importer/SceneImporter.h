#ifndef _SCENEIMPORTER_H_
#define _SCENEIMPORTER_H_
#include <map> 
#include "Importer.h"

class SceneImporter
{
private:
	void LoadImportedFilesRegister() const;
	void SaveImportedFilesRegister() const;
	std::map<std::string, std::string> imported_file_register;
	const char * const REGISTER_PATH = ".//Library//file_register.txt";
};
#endif // !_SCENEIMPORTER_H_

