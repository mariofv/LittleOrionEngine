#ifndef _IMPORTER_H_
#define _IMPORTER_H_
#include "Globals.h"
#include "Application.h"
#include "Module/ModuleFileSystem.h"

#include <string>

class Importer {
public:
	virtual bool Import(const char* file, std::string& output_file) = 0;
	//virtual bool Import(const void* buffer, size_t size, std::string & output_file) = 0;

};
#endif // !_IMPORTER_H_

