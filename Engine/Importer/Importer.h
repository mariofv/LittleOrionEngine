#ifndef _IMPORTER_H_
#define _IMPORTER_H_
#include <string>
#include "Globals.h"
class Importer {
public:
	virtual bool Import(const char* file, std::string& output_file)  const= 0;
	//virtual bool Import(const void* buffer, size_t size, std::string & output_file) = 0;
};
#endif // !_IMPORTER_H_

